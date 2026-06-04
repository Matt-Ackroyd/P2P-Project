#include "PrimaryClient.h"

PrimaryClient* PrimaryClient::instancePtr = nullptr;
mutex PrimaryClient::mtx;

// Static method to get the Singleton instance
PrimaryClient* PrimaryClient::getInstance() {
    if (instancePtr == nullptr) {
        lock_guard<mutex> lock(mtx);
        if (instancePtr == nullptr) {
            instancePtr = new PrimaryClient();

            // Innital Values 
            instancePtr->keyPair = NULL;
            // Load from file later
            instancePtr->clientID.GenerateNewID();
        }
    }
    return instancePtr;
}

EVP_PKEY* PrimaryClient::getKeyPair() {
    // TEMP TODO Save KEYPAIR
    if (this->keyPair == NULL) {
        this->keyPair = EVP_PKEY_Q_keygen(NULL, NULL, "ML-KEM-1024");
    }
    return this->keyPair;
}

UUID PrimaryClient::getClientID() {
    return this->clientID;
}


int PrimaryClient::registerNewUser(UUID id, unsigned char* secret) {
    RemoteUser test(id, secret);
    // TODO link remote user connection  
    return 1;  
}
