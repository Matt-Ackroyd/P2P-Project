#include "UDPConnection.h"
#include <PrimaryClient.h>

UDPConnection::UDPConnection(unsigned char* sharedSecret) {
    this->sharedSecret = sharedSecret;
}

UDPConnection::~UDPConnection() {
    delete[] this->sharedSecret;
}

void UDPConnection::connectTo(char const *addr) {
    struct sockaddr_in servaddr;
    
    // clear servaddr
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_addr.s_addr = inet_addr(addr);
    servaddr.sin_port = htons(this->SendingPort);
    servaddr.sin_family = AF_INET;

    this->sock = socket(AF_INET, SOCK_DGRAM, 0);

    // connect to server
    if(connect(this->sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("\n Error : Connect Failed \n");
        exit(0);
    }
    cout << "Connected\n";
}

void UDPConnection::send(unsigned char* data, int datalen) {
    // // Update SeqNumbers
    // packet.setSeqNum(this->currentSeqNum);
    // this->currentSeqNum += 1;

    // // Add Packet to buffer until its acknowlaged
    // this->sendingBuffer.push_back(packet);

    // // Send Over Socket


    // TEMP PLEASE REMOVE TODO
    unsigned char key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,14};
    this->sharedSecret = key;

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
    int packetlen = packetToSend->serialize(ciphertext, datalen, iv, tag);
    sendto(this->sock, packetToSend->getData(), packetlen, 0, (struct sockaddr*)NULL, sizeof((struct sockaddr*)NULL));
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
    

    int packetlen = packet->serialize(data, ML_KEM_HANDSHAKE_RANDSIZE + ML_KEM_KEYLENGTH, NULL, NULL);
    sendto(this->sock, packet->getData(), packetlen, 0, (struct sockaddr*)NULL, sizeof((struct sockaddr*)NULL));
    delete packet;
}


unsigned char* UDPConnection::getSharedSecret() {
    return this->sharedSecret;
}