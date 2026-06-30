#include "UDPConnection.h"
#include "PrimaryClient.h"
#include "ML-KEM_Handshake.h"

UDPConnection::UDPConnection() {
    this->sock = PrimaryClient::getInstance()->socketfd;
}

UDPConnection::~UDPConnection() {
    delete[] this->sharedSecret;
}

void UDPConnection::setAddr(char const *addr, int port) {
    // clear servaddr
    memset(&this->connectionAddr, 0, sizeof(this->connectionAddr));
    this->connectionAddr.sin_addr.s_addr = inet_addr(addr);
    this->connectionAddr.sin_port = htons(port);
    this->connectionAddr.sin_family = AF_INET;
}

void UDPConnection::sendAck(int seqNum) {
    Packet ack(seqNum, PacketType::ACK, PrimaryClient::getInstance()->getClientID());

    int packetlen = ack.serialize(NULL, 0, NULL, NULL);
    sendto(this->sock, ack.getData(), packetlen, 0, (struct sockaddr*)&connectionAddr, sizeof(connectionAddr));
}

// Send an Empty packet in order to keep the connection going 
void UDPConnection::sendKeepAlive() {
    unsigned char* keepAlive[0];
    Packet packet(-1, PacketType::KEEP_ALIVE, PrimaryClient::getInstance()->getClientID());

    int packetlen = packet.serialize((char*)keepAlive, 0, NULL, NULL);
    sendto(this->sock, packet.getData(), packetlen, 0, (struct sockaddr*)&connectionAddr, sizeof(connectionAddr)); 
}

void UDPConnection::sendEncrypted(unsigned char* data, int datalen) {

    // If this user hasn't created a shared secret yet then we can't send encrypted data
    if (this->sharedSecret == NULL) {
        return;
    }

    Packet *packetToSend = new Packet(newSeqNum(), PacketType::PACKET, PrimaryClient::getInstance()->getClientID());

    // AAD Gen for the senderID and incoming length of the data
    unsigned char aad[UUID_BYTE_SIZE + sizeof(datalen)];
    memcpy(aad, packetToSend->packetAuthorID.getRaw(), UUID_BYTE_SIZE);
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
    Packet* requestPacket = ML_KEM_Handshake::startHandshake(this->handshakeRandBuffer, client->getKeyPair(), client->getClientID(), newSeqNum());
    addPacketToOutgoingQueue(requestPacket);
}

void UDPConnection::sendPacket(Packet* packet) {
    sendto(this->sock, packet->getData(), packet->getPacketlength(), 0, (struct sockaddr*)&connectionAddr, sizeof(connectionAddr)); 
}

unsigned char* UDPConnection::getSharedSecret() {
    return this->sharedSecret;
}
void UDPConnection::setSharedSecret(unsigned char* secret) {
    this->sharedSecret = secret;
}

    
void UDPConnection::receivedAck(int seqNum) {
    for (auto packet: outgoingBuffer) {
        if (packet->getSeqNum() > seqNum) {
            return;
        }
        // Get the oldest packet & delete it
        outgoingBuffer.pop_front();
        delete packet;
    }
}

void UDPConnection::addPacketToIncomingQueue(Packet* incomingPacket) {
    // if the buffer is empty just add the packet to it
    if (incommingBuffer.empty()) {
        incommingBuffer.push_back(incomingPacket);
        return;
    }
    // if its not empty then insert it in sorted order
    // Loop until the new packet's seq num is less than the element that we are looking, we then insert it before that element
    for (std::deque<Packet*>::iterator it = incommingBuffer.begin(); it != incommingBuffer.end(); ++it){
        Packet* packetInQueue = *it; 
        
        if (packetInQueue->getSeqNum() > incomingPacket->getSeqNum()) {
            incommingBuffer.insert(it, incomingPacket);
            return;
        }
    
    }
}

void UDPConnection::addPacketToOutgoingQueue(Packet* outgoingPacket) {
    this->outgoingBuffer.push_back(outgoingPacket);
}

// Returns a seqnum and increments it by one for the next call
int UDPConnection::newSeqNum() {
    // TODO ADD mtuxed Guard to prevent race conditions
    int output = this->outgoingSeqNum;
    this->outgoingSeqNum++;
    return output;
}

std::deque<Packet*>* UDPConnection::getOutgoingBuffer() {
    return &this->outgoingBuffer;
}
std::deque<Packet*>* UDPConnection::getIncomingBuffer() {
    return &this->incommingBuffer;
}