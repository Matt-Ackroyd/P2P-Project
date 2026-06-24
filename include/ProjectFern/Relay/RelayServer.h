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

// Adds a user to the data base with a hash of the salted password
// Requires a Encrypted Connection Beforehand 
void onRegisterUser(ID userID, std::string password);

// Updates the stored connection info of a given user, as long as the password hash works
// Requires a Encrypted Connection Beforehand 
void onUpdateUserConnectionInfo(ID userID, std::string password, bool status);

// returns a users connection info
// Does not require an Encrypted Connection
void onUserConnectionInfoReqest(ID userID);


void handleIncomingRequest(SOCKTYPE clientSocket, sockaddr_in clientAddress);