#include "UDPConnection.h"
#include <PrimaryClient.h>

UDPConnection::UDPConnection(unsigned char* sharedSecret) {
    this->sharedSecret = sharedSecret;
    this->sock = PrimaryClient::getInstance()->socketfd;
    //Temp set addr
    //this->setAddr("127.0.0.1");
    this->setAddr("192.168.0.17");
}

UDPConnection::~UDPConnection() {
    delete[] this->sharedSecret;
}

void UDPConnection::setAddr(char const *addr) {
    // clear servaddr
    //bzero(&this->connectionAddr, sizeof(this->connectionAddr));
    this->connectionAddr.sin_addr.s_addr = inet_addr(addr);
    this->connectionAddr.sin_port = htons(this->SendingPort);
    this->connectionAddr.sin_family = AF_INET;
}

void UDPConnection::send(unsigned char* data, int datalen) {
    // // Update SeqNumbers
    // packet.setSeqNum(this->currentSeqNum);
    // this->currentSeqNum += 1;

    // // Add Packet to buffer until its acknowlaged
    // this->sendingBuffer.push_back(packet);

    // // Send Over Socket


    //Gen IV
    Packet *packetToSend = new Packet(123, PacketType::PACKET);

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

void UDPConnection::sendConnectionRequest() {
    Packet *packet = new Packet(-1, PacketType::CONNECTION_REQUEST);
    unsigned char data[ML_KEM_HANDSHAKE_RANDSIZE + ML_KEM_KEYLENGTH];

    // Generate Random Number for handshake & store it for later in Primary Client
    RAND_bytes(PrimaryClient::getInstance()->handShakeRand, ML_KEM_HANDSHAKE_RANDSIZE);
    memcpy(data, PrimaryClient::getInstance()->handShakeRand, ML_KEM_HANDSHAKE_RANDSIZE);

    // Get Public key & place in data(offset by the randsize)
    size_t publen = ML_KEM_KEYLENGTH;
    EVP_PKEY_get_raw_public_key(PrimaryClient::getInstance()->getKeyPair(), 
        data+ML_KEM_HANDSHAKE_RANDSIZE, &publen);
    

    int packetlen = packet->serialize((char*)data, ML_KEM_HANDSHAKE_RANDSIZE + ML_KEM_KEYLENGTH, NULL, NULL);
    sendto(this->sock, packet->getData(), packetlen, 0, (struct sockaddr*)&connectionAddr, sizeof(connectionAddr));
    delete packet;
}


unsigned char* UDPConnection::getSharedSecret() {
    return this->sharedSecret;
}