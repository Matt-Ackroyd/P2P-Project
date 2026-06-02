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

__int128_t PrimaryClient::getClientID() {
    return this->clientID;
}


int PrimaryClient::registerNewUser(__int128 id, unsigned char* secret) {
    RemoteUser *test = new RemoteUser(id);
    test->connection.sharedSecret = secret;
    // TODO link remote user connection    
}