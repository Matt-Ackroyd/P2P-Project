#include "RelayClient.h"


SOCKTYPE RelayClient::EstablishEncryption(std::string relayAddr, int relayPort, unsigned char* sharedSecretBuffer) {
    // creating socket
    SOCKTYPE socketfd = socket(AF_INET, SOCK_STREAM, 0);

    // specifying address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(relayPort);
    serverAddress.sin_addr.s_addr = inet_addr(relayAddr.c_str());

    // sending connection request
    connect(socketfd, (struct sockaddr*)&serverAddress,
            sizeof(serverAddress));

    // sending data
    unsigned char myRand[ML_KEM_HANDSHAKE_RANDSIZE];
    //ML_KEM_Handshake::startHandshake(myRand, PrimaryClient::getInstance()->getKeyPair(), socketfd, serverAddress, PrimaryClient::getInstance()->getClientID(), -1);

    // Wait for response
    char buffer[3000] = {0};
    recv(socketfd, buffer, sizeof(buffer), 0);

    Packet incomingPacket(-1, PacketType::NONE, PrimaryClient::getInstance()->getClientID());
    int datalen = incomingPacket.deserialize(buffer);

    ML_KEM_Handshake::onReply(&incomingPacket, PrimaryClient::getInstance()->getKeyPair(), myRand, sharedSecretBuffer);
    
    return socketfd;


}

void SendRelayRegisterRequest(std::string relayAddr, int relayPort, PacketType requestType) {
    std::string path = KNOWN_RELAY_PATH + relayAddr + ":" + (char*)relayPort + "/" + "passwordHash.bin";

    // If a password exists for this relay load it, if not then generate one
    int datalen = PASSWORD_BYTE_SIZE;
    char password[PASSWORD_BYTE_SIZE];
    if (requestType == PacketType::RELAY_REGISTER) {
        RAND_bytes((unsigned char*)password, PASSWORD_BYTE_SIZE);
        ConfigLoader::getInstance()->WriteBinaryFile("Configs/PrimaryClient/KnownRelays/" + relayAddr , password, PASSWORD_BYTE_SIZE);
    } else {
        ConfigLoader::getInstance()->ReadBinaryFile("Configs/PrimaryClient/KnownRelays/" + relayAddr, password, PASSWORD_BYTE_SIZE);
    }

    Packet request(-1, requestType, PrimaryClient::getInstance()->getClientID());

    // AAD Gen for the senderID and incoming length of the data
    unsigned char aad[UUID_BYTE_SIZE + sizeof(datalen)];
    memcpy(aad, request.packetAuthorID.getRaw(), UUID_BYTE_SIZE);
    memcpy(aad+UUID_BYTE_SIZE, &datalen, sizeof(datalen));


    // IV GEN
    unsigned char iv[AES_256_IV_LENGTH];
    if (!RAND_bytes(iv, AES_256_IV_LENGTH)) {
        handleErrors();
    }

    // Encryption
    unsigned char ciphertext[datalen];
    unsigned char tag[AES_256_GCM_TAG_LENGTH];
    if (symmetricEncryption((unsigned char*)password, datalen, aad, sizeof(aad), sharedsecret, iv, AES_256_IV_LENGTH, ciphertext, tag) < 0) {
        // Failed
        return;
    }
    
    // Encapsulate in a packet & send
    int packetlen = request.serialize((char*)ciphertext, datalen, iv, tag);

    send(socketfd, request.getData(), packetlen, 0);
    // closing socket
    closesocket(socketfd);
}


void RelayClient::StartHandshake(std::string relayAddr, int relayPort) {
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(relayPort);
    serverAddress.sin_addr.s_addr = inet_addr(relayAddr.c_str());

    unsigned char myRand[ML_KEM_HANDSHAKE_RANDSIZE];
    Packet* packet = ML_KEM_Handshake::startHandshake(myRand, PrimaryClient::getInstance()->getKeyPair(), PrimaryClient::getInstance()->getClientID(), -1);

    //Save Rand To File
    ConfigLoader::WriteBinaryFile(KNOWN_RELAY_PATH + relayAddr + ":" + (char*)relayPort + "/" + "random.bin", (char*)myRand, ML_KEM_HANDSHAKE_RANDSIZE);

    sendto(PrimaryClient::getInstance()->socketfd, packet->getData(), packet->getPacketlength(), 0, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    delete packet;
}

void onRelayHandshakeResponse(Packet* incomingPacket, sockaddr_in addr) {
    std::string ip = inet_ntoa(addr.sin_addr);
    int port = ntohs(addr.sin_port);
    
    unsigned char myRand[ML_KEM_HANDSHAKE_RANDSIZE];
    ConfigLoader::ReadBinaryFile(KNOWN_RELAY_PATH + ip + ":" + (char*)port + "/" + "random.bin", (char*)myRand, ML_KEM_HANDSHAKE_RANDSIZE);

    unsigned char sharedSecretBuffer[SHAW_256_HASH_SIZE];
    ML_KEM_Handshake::onReply(incomingPacket, PrimaryClient::getInstance()->getKeyPair(), myRand, sharedSecretBuffer);

}

// returns a given users connection info
// Does not require an Encrypted Connection
void RelayClient::UserConnectionInfoReqest(SOCKTYPE socketfd, std::string relayAddr, int relayPort, ID requestedUserID, ID* yourID) {
    // specifying address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(relayPort);
    serverAddress.sin_addr.s_addr = inet_addr(relayAddr.c_str());

    Packet packet(-1, PacketType::RELAY_USER_INFO, yourID);
    int packetlen = packet.serialize((char*)requestedUserID.getRaw(), UUID_BYTE_SIZE, NULL, NULL);
    sendto(socketfd, packet.getData(), packetlen, 0, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
}
