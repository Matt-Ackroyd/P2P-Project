#pragma once
#include <iostream>
#include <filesystem>
#include <fstream>
#include <thread>
#include "ID.h"
#include "Encryption.h"
#include "ML-KEM_Handshake.h"
#include "socketMacro.h"

#define PATH_TO_USER_FILES "relay_users/"
#define PASSWORD_BYTE_SIZE 32
#define CONNECTION_INFO_SIZE 6

class RelayServer {
public:
    // Adds a user to the data base with a hash of the salted password
    // Requires a Encrypted Connection Beforehand 
    static void onRegisterUser(ID* userID, unsigned char* data, sockaddr_in* cliaddr);

    // Updates the stored connection info of a given user, as long as the password hash works
    // Requires a Encrypted Connection Beforehand 
    static void onUpdateUserConnectionInfo(ID* userID, unsigned char* data, sockaddr_in* cliaddr);

    // returns a users connection info
    // Does not require an Encrypted Connection
    static void onUserConnectionInfoReqest(Packet *packet, SOCKTYPE socketfd, sockaddr_in* cliaddr, socklen_t clientlen);

    static void catchTcpConnection(SOCKTYPE clientSocket, sockaddr_in clientAddress);
    static void handleTcpConnection(SOCKTYPE clientSocket, sockaddr_in clientAddress);

    static void handleIncomingRequest(SOCKTYPE clientSocket, sockaddr_in clientAddress);

    static void UdpHandler(int udpPort);
};