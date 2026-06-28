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
    //bzero(&this->connectionAddr, sizeof(this->connectionAddr));
    this->connectionAddr.sin_addr.s_addr = inet_addr(addr);
    this->connectionAddr.sin_port = htons(port);
    this->connectionAddr.sin_family = AF_INET;
}

void UDPConnection::sendEncrypted(unsigned char* data, int datalen) {

    // If this user hasn't created a shared secret yet then we can't send encrypted data
    if (this->sharedSecret == NULL) {
        return;
    }

    //Gen IV
    Packet *packetToSend = new Packet(123, PacketType::PACKET, PrimaryClient::getInstance()->getClientID());

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
    int packetlen = packetToSend->serialize((char*)ciphertext, datalen, iv, tag);
    sendto(this->sock, packetToSend->getData(), packetlen, 0, (struct sockaddr*)&connectionAddr, sizeof(connectionAddr));
    delete packetToSend;
}

// Send an Empty packet in order to keep the connection going 
void UDPConnection::sendKeepAlive() {
    unsigned char* keepAlive[0];
    Packet packet(0, PacketType::KEEP_ALIVE, PrimaryClient::getInstance()->getClientID());

    int packetlen = packet.serialize((char*)keepAlive, 0, NULL, NULL);
    sendto(this->sock, packet.getData(), packetlen, 0, (struct sockaddr*)&connectionAddr, sizeof(connectionAddr));
}

void UDPConnection::sendHandshakeRequest() {
    PrimaryClient* client = PrimaryClient::getInstance();
    ML_KEM_Handshake::startHandshake(this->handshakeRandBuffer, client->getKeyPair(), client->socketfd, connectionAddr, client->getClientID());
}


unsigned char* UDPConnection::getSharedSecret() {
    return this->sharedSecret;
}
void UDPConnection::setSharedSecret(unsigned char* secret) {
    this->sharedSecret = secret;
}