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
    ML_KEM_Handshake::startHandshake(myRand, PrimaryClient::getInstance()->getKeyPair(), socketfd, &serverAddress, PrimaryClient::getInstance()->getClientID());

    // Wait for response
    char buffer[1024] = {0};
    recv(socketfd, buffer, sizeof(buffer), 0);

    Packet incomingPacket(-1, PacketType::NONE, PrimaryClient::getInstance()->getClientID());
    int datalen = incomingPacket.deserialize(buffer);

    sharedSecretBuffer = ML_KEM_Handshake::onReply(&incomingPacket, PrimaryClient::getInstance()->getKeyPair(), myRand);
    
    return socketfd;


}

void RelayClient::TcpRequest(std::string relayAddr, int relayPort, PacketType requestType) {
    unsigned char* sharedsecret;
    SOCKTYPE socketfd = EstablishEncryption(relayAddr, relayPort, sharedsecret);

    //TODO Need to load password for this relay from file when requesting a modification rather than creation
    int datalen = PASSWORD_BYTE_SIZE;
    unsigned char password[PASSWORD_BYTE_SIZE];
    RAND_bytes(password, PASSWORD_BYTE_SIZE);

    Packet request(-1, requestType, PrimaryClient::getInstance()->getClientID());

    // AAD Gen for the senderID and incoming length of the data
    unsigned char aad[UUID_BYTE_SIZE + sizeof(request)];
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
    if (!symmetricEncryption(password, datalen, aad, sizeof(aad), sharedsecret, iv, AES_256_IV_LENGTH, ciphertext, tag)) {
        // Failed
        return;
    }
    
    // Encapsulate in a packet & send
    int packetlen = request.serialize((char*)ciphertext, datalen, iv, tag);

    send(socketfd, request.getData(), packetlen, 0);
    // closing socket
    closesocket(socketfd);
}

// Requests this user to be added to the database with a hash of the salted password
// Requires a Encrypted Connection Beforehand 
void RelayClient::RegisterUser(std::string relayAddr, int relayPort) {
    TcpRequest(relayAddr, relayPort, PacketType::RELAY_REQUEST_USER_REGISTRATION);
}

// Requests to update the database with this users connection infomation
// Requires a Encrypted Connection Beforehand 
void RelayClient::UpdateUserConnectionInfo(std::string relayAddr, int relayPort) {
    TcpRequest(relayAddr, relayPort, PacketType::RELAY_REQUEST_UPDATE_CONNECTION_INFO);
}

// returns a given users connection info
// Does not require an Encrypted Connection
void RelayClient::UserConnectionInfoReqest(std::string relayAddr, int relayPort, ID userID) {

}
