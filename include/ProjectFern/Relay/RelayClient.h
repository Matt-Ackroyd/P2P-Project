#pragma once
//#include "RelayServer.h"
#include "PrimaryClient.h"
#include "ConfigLoader.h"
#include "ML-KEM_Handshake.h"

#define PASSWORD_BYTE_SIZE 32
#define CONNECTION_INFO_SIZE 6
#define KNOWN_RELAY_PATH "Configs/PrimaryClient/KnownRelays/"

class RelayClient {
private:
    static void SendRelayRegisterRequest(std::string relayAddr, int relayPort, unsigned char* sharedsecret);

public:
    // Requests this user to be added to the database with a hash of the salted password
    // Requires a Encrypted Connection Beforehand 
    static void RegisterUser(std::string relayAddr, int relayPort);

    // returns a given users connection info
    // Does not require an Encrypted Connection
    static void UserConnectionInfoReqest(SOCKTYPE socketfd, std::string relayAddr, int relayPort, ID requestedUserID, ID* YourID);
    
    static void onRelayHandshakeResponse(Packet* incomingPacket, sockaddr_in addr);
};