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
    __int128_t clientID;
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
    int registerNewUser(__int128_t id, unsigned char* secret);
    EVP_PKEY* getKeyPair();

    __int128_t getClientID();

    // A Temparary buffer for the current ongoing handshake, TODO allow for multiple handshakes to be ongoing at once
    unsigned char handShakeRand[ML_KEM_HANDSHAKE_RANDSIZE];
};