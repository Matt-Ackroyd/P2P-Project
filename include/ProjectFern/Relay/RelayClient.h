#pragma once
#include "RelayServer.h"
#include "PrimaryClient.h"

class RelayClient {
private:
    static SOCKTYPE EstablishEncryption(std::string relayAddr, int relayPort, unsigned char* sharedSecretBuffer);
    static void TcpRequest(std::string relayAddr, int relayPort, PacketType requestType);
public:
    // Requests this user to be added to the database with a hash of the salted password
    // Requires a Encrypted Connection Beforehand 
    static void RegisterUser(std::string relayAddr, int relayPort);

    // Requests to update the database with this users connection infomation
    // Requires a Encrypted Connection Beforehand 
    static void UpdateUserConnectionInfo(std::string relayAddr, int relayPort);

    // returns a given users connection info
    // Does not require an Encrypted Connection
    static void UserConnectionInfoReqest(std::string relayAddr, int relayPort, ID userID);
};