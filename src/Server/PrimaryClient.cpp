#include "PrimaryClient.h"

PrimaryClient* PrimaryClient::instancePtr = nullptr;
mutex PrimaryClient::mtx;

// Static method to get the Singleton instance
PrimaryClient* PrimaryClient::getInstance() {
    if (instancePtr == nullptr) {
        lock_guard<mutex> lock(mtx);
        if (instancePtr == nullptr) {
            instancePtr = new PrimaryClient();

            // TEMP
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

int PrimaryClient::registerUser() {
    return 1;
}