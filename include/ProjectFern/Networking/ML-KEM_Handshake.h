#include "Encryption.h"
#include "Packet.h"
#include "socketMacro.h"


unsigned char* onML_KEM_HandshakeRequest(Packet* packet, SOCKTYPE socketfd, sockaddr_in *returnAdress, socklen_t returnLen);

unsigned char* onML_KEM_HandshakeReply(Packet* packet, EVP_PKEY* KeyPair, unsigned char* random);