#pragma once
#include "Encryption.h"
#include "Packet.h"
#include "socketMacro.h"

class ML_KEM_Handshake {
public:
    static Packet* startHandshake(unsigned char* randomBuffer, EVP_PKEY* keyPair, std::string* yourID, int seqenceNumber, EVP_PKEY* yourDSAkey);

    static Packet* onRequest(Packet* packet, std::string* yourID, unsigned char* outputedSecret, int seqenceNumber, EVP_PKEY* yourDSAkey, EVP_PKEY** theirDSAkey, PacketType type = PacketType::HANDSHAKE_RESPONSE);

    static int onReply(Packet* packet, EVP_PKEY* KeyPair, unsigned char* random, unsigned char* outputedSecret, EVP_PKEY** theirDSAkey);
};
