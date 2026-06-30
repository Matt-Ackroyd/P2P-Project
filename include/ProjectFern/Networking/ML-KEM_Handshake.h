#pragma once
#include "Encryption.h"
#include "Packet.h"
#include "socketMacro.h"

class ML_KEM_Handshake {
public:
    static Packet* startHandshake(unsigned char* randomBuffer, EVP_PKEY* keyPair, ID* yourID, int seqenceNumber);

    static Packet* onRequest(Packet* packet, ID* yourID, unsigned char* outputedSecret, int seqenceNumber);

    static int onReply(Packet* packet, EVP_PKEY* KeyPair, unsigned char* random, unsigned char* outputedSecret);
};