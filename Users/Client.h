// TODO: Needs Better name
// Acts as class that stores information about this particular client, where user refers to other clients 
#pragma once
#include "Server.h"
#include "../UDPConnection/ReceivingPackets/IncomingHandler.h"
#include <vector>
#include <openssl/evp.h>
#include <mutex>

using namespace std;

class Client {
private:
    vector<Server> allServers;
    string defaultUsername;
    IncomingHandler incomingHandler;
    EVP_PKEY *keyPair;

    // Static pointer to the Singleton instance
    static Client* instancePtr;
    // Mutex to ensure thread safety
    static mutex mtx;
    // Private Constructor
    Client() {}
public:
    // Deleting the copy constructor to prevent copies
    Client(const Client& obj) = delete;
    static Client* getInstance();
    int registerUser();
    EVP_PKEY* getKeyPair();
};