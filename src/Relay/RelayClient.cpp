#include "RelayClient.h"



void RelayClient::SendRelayRegisterRequest(std::string relayAddr, int relayPort, unsigned char* sharedsecret) {
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(relayPort);
    serverAddress.sin_addr.s_addr = inet_addr(relayAddr.c_str());

    std::string path = KNOWN_RELAY_PATH + relayAddr + "-" + std::to_string(relayPort) + "/" + "passwordHash.bin";
    
    // If a password exists for this relay load it, if not then generate one
    int datalen = PASSWORD_BYTE_SIZE;
    char password[PASSWORD_BYTE_SIZE];
    if (!std::filesystem::exists(std::filesystem::path(path))) {
        RAND_bytes((unsigned char*)password, PASSWORD_BYTE_SIZE);
        ConfigLoader::getInstance()->WriteBinaryFile(path , password, PASSWORD_BYTE_SIZE);
    } else {
        ConfigLoader::getInstance()->ReadBinaryFile(path, password, PASSWORD_BYTE_SIZE);
    }

    Packet request(-1, PacketType::RELAY_REGISTER_REQUEST, PrimaryClient::getInstance()->getClientID());

    // AAD Gen for the senderID and incoming length of the data
    unsigned char aad[UUID_BYTE_SIZE + sizeof(datalen)];
    unsigned char uuid[UUID_BYTE_SIZE];
    ID::BytesFromString(request.packetAuthorID, uuid);
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
    if (symmetricEncryption((unsigned char*)password, datalen, aad, sizeof(aad), sharedsecret, iv, AES_256_IV_LENGTH, ciphertext, tag) < 0) {
        // Failed
        return;
    }
    
    // Encapsulate in a packet & send
    int packetlen = request.serialize((char*)ciphertext, datalen, iv, tag);

    sendto(PrimaryClient::getInstance()->socketfd, request.getData(), packetlen, 0, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

}


void RelayClient::RegisterUser(std::string relayAddr, int relayPort) {
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(relayPort);
    serverAddress.sin_addr.s_addr = inet_addr(relayAddr.c_str());

    unsigned char myRand[ML_KEM_HANDSHAKE_RANDSIZE];
    Packet* packet = ML_KEM_Handshake::startHandshake(myRand, PrimaryClient::getInstance()->getKeyPair(), PrimaryClient::getInstance()->getClientID(), -1, NULL);

    //Save Rand To File
    ConfigLoader::WriteBinaryFile(KNOWN_RELAY_PATH + relayAddr + "-" + std::to_string(relayPort) + "/" + "random.bin", (char*)myRand, ML_KEM_HANDSHAKE_RANDSIZE);

    int a = sendto(PrimaryClient::getInstance()->socketfd, packet->getData(), packet->getPacketlength(), 0, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    int error = WSAGetLastError();
    delete packet;
}

void RelayClient::onRelayHandshakeResponse(Packet* incomingPacket, sockaddr_in addr) {
    std::string ip = inet_ntoa(addr.sin_addr);
    int port = ntohs(addr.sin_port);
    
    unsigned char myRand[ML_KEM_HANDSHAKE_RANDSIZE];
    ConfigLoader::ReadBinaryFile(KNOWN_RELAY_PATH + ip + "-" + std::to_string(port) + "/" + "random.bin", (char*)myRand, ML_KEM_HANDSHAKE_RANDSIZE);

    unsigned char sharedSecretBuffer[SHAW_256_HASH_SIZE];
    ML_KEM_Handshake::onReply(incomingPacket, PrimaryClient::getInstance()->getKeyPair(), myRand, sharedSecretBuffer, NULL);

    RelayClient::SendRelayRegisterRequest(ip, port, sharedSecretBuffer);
}

// returns a given users connection info
// Does not require an Encrypted Connection
void RelayClient::UserConnectionInfoRequest(SOCKTYPE socketfd, std::string relayAddr, int relayPort, std::string requestedUserID, std::string* yourID) {
    // specifying address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(relayPort);
    serverAddress.sin_addr.s_addr = inet_addr(relayAddr.c_str());

    Packet packet(-1, PacketType::RELAY_USER_INFO, yourID);
    unsigned char uuid[UUID_BYTE_SIZE];
    ID::BytesFromString(requestedUserID, uuid);
    int packetlen = packet.serialize((char*)uuid, UUID_BYTE_SIZE, NULL, NULL);
    sendto(socketfd, packet.getData(), packetlen, 0, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
}
