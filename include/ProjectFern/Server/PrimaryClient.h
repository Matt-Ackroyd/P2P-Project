// TODO: Needs Better name
// Acts as class that stores information about this particular client, where user refers to other clients 
#pragma once
#include "Server.h"
#include "IncomingHandler.h"
#include <vector>
#include <openssl/evp.h>
#include <mutex>

using namespace std;

class PrimaryClient {
private:
    vector<Server> allServers;
    string defaultUsername;
    //IncomingHandler incomingHandler;
    EVP_PKEY *keyPair;

    // Static pointer to the Singleton instance
    static PrimaryClient* instancePtr;
    // Mutex to ensure thread safety
    static mutex mtx;
    // Private Constructor
    PrimaryClient() {}
public:
    // Deleting the copy constructor to prevent copies
    PrimaryClient(const PrimaryClient& obj) = delete;
    static PrimaryClient* getInstance();
    int registerUser();
    EVP_PKEY* getKeyPair();
};