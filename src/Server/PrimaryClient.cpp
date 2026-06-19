#include "PrimaryClient.h"

PrimaryClient* PrimaryClient::instancePtr = nullptr;
std::mutex PrimaryClient::mtx;

// Static method to get the Singleton instance
PrimaryClient* PrimaryClient::getInstance() {
    if (instancePtr == nullptr) {
        std::lock_guard<std::mutex> lock(mtx);
        if (instancePtr == nullptr) {
            instancePtr = new PrimaryClient();
        }
    }
    return instancePtr;
}

void PrimaryClient::init() {
    // Innital Values 
    instancePtr->keyPair = NULL;
    // Load from file later
    instancePtr->clientID.GenerateNewID();
    std::cout << instancePtr->clientID.get() << "\n";

    // Socket Compatibility Stuff
    #ifdef _WIN32
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2,2), &wsaData);
    #endif

    instancePtr->socketfd = socket(AF_INET, SOCK_DGRAM, 0);  
    //setsockopt(instancePtr->socketfd, SOL_SOCKET, SO_REUSEADDR, (const char*)1, sizeof(int));
}

EVP_PKEY* PrimaryClient::getKeyPair() {
    // TEMP TODO Save KEYPAIR
    if (this->keyPair == NULL) {
        this->keyPair = EVP_PKEY_Q_keygen(NULL, NULL, "ML-KEM-1024");
    }
    return this->keyPair;
}

ID PrimaryClient::getClientID() {
    return this->clientID;
}


int PrimaryClient::registerNewUser(ID id, unsigned char* secret) {
    // Guard clause to not add oneself as a new user
    if (id.get() == this->clientID.get()) {
        std::cout << "Cannot Register Yourself\n";
        return -1;
    }
    
    // Guard Clause to not overwrite a user
    if (this->knownConnections[id.get()] != 0) {
        std::cout << "User " << id.get() << " already Exists\n";
        return -1;
    }

    // Create a temperary user to asosiate incoming packets from this user will be lost on reset if not proporly added to a server
    RemoteUser* test = new RemoteUser(id, secret);
    // TODO link remote user connection  

    // add to the list of all known connections
    std::cout << "New User Added: " << id.get() << " \n";
    this->knownConnections[id.get()] = test;
    return 1;  // return sucsess 
}

RemoteUser* PrimaryClient::getUser(std::string userID) {
    return this->knownConnections[userID];
}


void PrimaryClient::addNewServer(std::string id, Server *server) {
    this->allServers[id] = server;
    emit CppInterface::instancePtr->addServer(server);
    
}