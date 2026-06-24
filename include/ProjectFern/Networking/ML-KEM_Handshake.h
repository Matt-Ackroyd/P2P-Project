#pragma once
#include "Encryption.h"
#include "Packet.h"
#include "socketMacro.h"

class ML_KEM_Handshake {
public:
    static void startHandshake(unsigned char* randomBuffer, EVP_PKEY* keyPair, SOCKTYPE socketfd, sockaddr_in* connectionAddr, ID* yourID);

    static unsigned char* onRequest(Packet* packet, SOCKTYPE socketfd, sockaddr_in *returnAdress, socklen_t returnLen, ID* yourID);

    static unsigned char* onReply(Packet* packet, EVP_PKEY* KeyPair, unsigned char* random);
};