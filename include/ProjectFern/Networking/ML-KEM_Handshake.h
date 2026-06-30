#pragma once
#include "Encryption.h"
#include "Packet.h"
#include "socketMacro.h"

class ML_KEM_Handshake {
public:
    static void startHandshake(unsigned char* randomBuffer, EVP_PKEY* keyPair, SOCKTYPE socketfd, sockaddr_in connectionAddr, ID* yourID, int seqenceNumber);

    static int onRequest(Packet* packet, SOCKTYPE socketfd, sockaddr_in returnAdress, ID* yourID, unsigned char* outputedSecret, int seqenceNumber);

    static int onReply(Packet* packet, EVP_PKEY* KeyPair, unsigned char* random, unsigned char* outputedSecret);
};