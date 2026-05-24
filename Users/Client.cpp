#include "Client.h"

// Static method to get the Singleton instance
Client* Client::getInstance() {
    if (instancePtr == nullptr) {
        lock_guard<mutex> lock(mtx);
        if (instancePtr == nullptr) {
            instancePtr = new Client();
        }
    }
    return instancePtr;
}

EVP_PKEY* Client::getKeyPair() {
    return this->keyPair;
}

int Client::registerUser() {
    
}