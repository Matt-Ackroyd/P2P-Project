#pragma once
#include <iostream>
#include <filesystem>
#include <fstream>
#include <thread>
#include "ID.h"
#include "Encryption.h"
#include "ML-KEM_Handshake.h"
#include "socketMacro.h"
#include "ConfigLoader.h"

#define PATH_TO_USER_FILES "relay/users/"
#define PASSWORD_BYTE_SIZE 32
#define CONNECTION_INFO_SIZE 6

class RelayServer {
    // Helper functions
    static void SaltAndHash(unsigned char* input, unsigned char* output);
    static std::filesystem::path UserPath(ID* userID);

    // returns a users connection info
    // Does not require an Encrypted Connection
    static void onUserConnectionInfoReqest(Packet *packet, SOCKTYPE socketfd, sockaddr_in* cliaddr, socklen_t clientlen);
    static void EstablishSharedSecret(Packet* handshakePacket, SOCKTYPE socketfd, sockaddr_in cliaddr);

    // Requires a SharedSecret/Encrypted Connection
    static void onRequestRegistration(Packet* incomingPacket, SOCKTYPE socketfd, sockaddr_in cliaddr);
    static void CreatePasswordHashFile(std::filesystem::path path, unsigned char* passwordData);
    static void UpdateConnectionInfo(std::filesystem::path path, unsigned char* passwordData, sockaddr_in cliaddr);
    
public:
    static ID relayID;
    static void UdpHandler(int udpPort);
};