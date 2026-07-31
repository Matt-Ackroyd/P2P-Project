#include "UDPConnection.h"
#include "PrimaryClient.h"
#include "ML-KEM_Handshake.h"

UDPConnection::UDPConnection() {
    this->sock = PrimaryClient::getInstance()->socketfd;
    

    this->mtx;
}

UDPConnection::~UDPConnection() {
    if (this->sharedSecret != NULL) {
        delete[] this->sharedSecret;
    }
}

void UDPConnection::setAddr(char const *addr, int port) {
    // clear servaddr
    memset(&this->connectionAddr, 0, sizeof(this->connectionAddr));
    this->connectionAddr.sin_addr.s_addr = inet_addr(addr);
    this->connectionAddr.sin_port = htons(port);
    this->connectionAddr.sin_family = AF_INET;
}

void UDPConnection::sendAck(std::string packetID) {
    Packet ack(PacketType::ACK, PrimaryClient::getInstance()->getClientID(), packetID);

    int packetlen = ack.serialize(NULL, 0, NULL, NULL);
    sendto(this->sock, ack.getData(), packetlen, 0, (struct sockaddr*)&connectionAddr, sizeof(connectionAddr));
}

void UDPConnection::sendHello() {
    Packet* hello = new Packet(PacketType::HELLO, PrimaryClient::getInstance()->getClientID());

    hello->serialize(NULL, 0, NULL, NULL);
    addPacketToOutgoingQueue(hello);
}

// Send an Empty packet in order to keep the connection going 
void UDPConnection::sendKeepAlive() {
    unsigned char* keepAlive[0];
    Packet packet(PacketType::KEEP_ALIVE, PrimaryClient::getInstance()->getClientID());

    int packetlen = packet.serialize((char*)keepAlive, 0, NULL, NULL);
    int a = sendto(this->sock, packet.getData(), packetlen, 0, (struct sockaddr*)&connectionAddr, sizeof(connectionAddr)); 
}

void UDPConnection::sendEncrypted(unsigned char* data, int datalen) {

    // If this user hasn't created a shared secret yet then we can't send encrypted data
    if (this->sharedSecret == NULL) {
        return;
    }

    Packet *packetToSend = new Packet(PacketType::PACKET, PrimaryClient::getInstance()->getClientID());

    // AAD Gen for the senderID and incoming length of the data
    unsigned char aad[UUID_BYTE_SIZE + sizeof(datalen)];
    unsigned char uuid[UUID_BYTE_SIZE];
    ID::BytesFromString(packetToSend->packetAuthorID, uuid);
    memcpy(aad, uuid, UUID_BYTE_SIZE);
    memcpy(aad+UUID_BYTE_SIZE, &datalen, sizeof(datalen));


    // IV GEN
    unsigned char iv[AES_256_IV_LENGTH];
    if (!RAND_bytes(iv, AES_256_IV_LENGTH)) {
        handleErrors();
    }

    // Encryption
    unsigned char ciphertext[datalen];
    unsigned char tag[AES_256_GCM_TAG_LENGTH];
    symmetricEncryption(data, datalen, aad, sizeof(aad), this->sharedSecret, iv, AES_256_IV_LENGTH, ciphertext, tag);
    
    
    // Encapsulate in a packet & send
    packetToSend->serialize((char*)ciphertext, datalen, iv, tag);
    addPacketToOutgoingQueue(packetToSend);
}


void UDPConnection::sendHandshakeRequest() {
    PrimaryClient* client = PrimaryClient::getInstance();
    Packet* requestPacket = ML_KEM_Handshake::startHandshake(this->handshakeRandBuffer, client->getKeyPair(), client->getClientID(), newSeqNum(), client->getDSAkey());
    addPacketToOutgoingQueue(requestPacket);
}

void UDPConnection::sendPacket(Packet* packet) {
    int a = sendto(this->sock, packet->getData(), packet->getPacketlength(), 0, (struct sockaddr*)&connectionAddr, sizeof(connectionAddr));
    std::cout << " Bind Return: " << a << "\n";
}

unsigned char* UDPConnection::getSharedSecret() {
    return this->sharedSecret;
}
void UDPConnection::setSharedSecret(unsigned char* secret) {
    this->sharedSecret = secret;
}

    
void UDPConnection::receivedAck(std::string packetID) { // TODO add mtx Guard to prevent race conditions
    mtx.lock();

    if (!this->outgoingBuffer.contains(packetID)) {
       return;
    }

    Packet* packet = this->outgoingBuffer[packetID];
    this->outgoingBuffer.erase(packetID);
    delete this->outgoingBuffer[packetID];

    mtx.unlock();
}


void UDPConnection::addPacketToOutgoingQueue(Packet* outgoingPacket) { // TODO add mtx Guard to prevent race conditions
    mtx.lock();
    outgoingBuffer[outgoingPacket->getPacketID()] = outgoingPacket;
    mtx.unlock();
}

void UDPConnection::addPacketToIncomingQueue(Packet *incomingPacket)
{
    incommingBuffer[incomingPacket->getPacketID()] = std::chrono::system_clock::now();
}

// Returns a seqnum and increments it by one for the next call
int UDPConnection::newSeqNum() {
    mtx.lock();
    int output = this->outgoingSeqNum;
    this->outgoingSeqNum++;
    mtx.unlock();
    return output;
}

void UDPConnection::sendServer(Server* server) {
    ServerContainer container(DataTypes::SERVER, server);

    this->sendEncrypted(container.getData(), container.getDataLen());
}

void UDPConnection::sendTextChannel(TextChannel* channel) {
    TextChannelContainer container(DataTypes::TEXT_CHANNEL, channel);

    this->sendEncrypted(container.getData(), container.getDataLen());
}

void UDPConnection::sendJoinRequest(std::string invitationCode) {
    sockaddr_in relay = PrimaryClient::getInstance()->getPreferedRelay();
    JoinRequest request(DataTypes::JOIN_REQUEST, invitationCode, relay.sin_addr.s_addr, relay.sin_port, 1);

    this->sendEncrypted(request.getData(), request.getDataLen());
}

void UDPConnection::sendAddUserToServerRequest(RemoteUser* user, Server* server) {
    UserContainer request(DataTypes::USER, user, server);

    this->sendEncrypted(request.getData(), request.getDataLen());
}

void UDPConnection::resetConnection()
{
    mtx.lock();
    this->outgoingBuffer.clear();    
    mtx.unlock();
    
    this->synced = false;
    
}


void UDPConnection::sendSyncRequest(Server* server) {
    PrimaryClient* client = PrimaryClient::getInstance();

    SyncRequest* channelRequest = new SyncRequest(*server->getID(), DataTypes::TEXT_CHANNEL, 0, MAX_SYNC_REQUEST);
    this->sendEncrypted(channelRequest->getData(), channelRequest->getDataLen());

    SyncRequest* userRequest = new SyncRequest(*server->getID(), DataTypes::USER, 0, MAX_SYNC_REQUEST);
    this->sendEncrypted(userRequest->getData(), userRequest->getDataLen());

    SyncRequest* messageRequest = new SyncRequest(*server->getID(), DataTypes::MESSAGETYPE, 0, MAX_SYNC_REQUEST);
    this->sendEncrypted(messageRequest->getData(), messageRequest->getDataLen());

    SyncRequest* fileRequest = new SyncRequest(*server->getID(), DataTypes::FILE_INDICATOR, 0, MAX_SYNC_REQUEST);
    this->sendEncrypted(fileRequest->getData(), fileRequest->getDataLen());

    this->syncRequests[channelRequest->getSyncID()] = channelRequest;
    this->syncRequests[userRequest->getSyncID()] = userRequest;
    this->syncRequests[messageRequest->getSyncID()] = messageRequest;
    this->syncRequests[fileRequest->getSyncID()] = fileRequest;
}
