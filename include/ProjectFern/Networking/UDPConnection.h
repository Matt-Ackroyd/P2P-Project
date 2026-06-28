#pragma once
#include <iostream>
#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include "socketMacro.h"
#include <thread>
#include <deque>
#include <fstream>
#include <vector>

#include "Packet.h"
#include "Encryption.h"


class UDPConnection {
private:
    struct sockaddr_in connectionAddr;
    int sock;
    int currentSeqNum; 

    unsigned char* sharedSecret = NULL;

    std::deque<Packet> sendingBuffer;
    std::vector<std::string> knownEndpoints;
public:    
    UDPConnection();
    ~UDPConnection();

    bool connected = false;

    unsigned char handshakeRandBuffer[ML_KEM_HANDSHAKE_RANDSIZE];
    
    void setAddr(char const *addr, int port);  
    void sendEncrypted(unsigned char* data, int datalen);
    void sendKeepAlive();

    void sendHandshakeRequest();

    unsigned char* getSharedSecret();
    void setSharedSecret(unsigned char* secret);
        
};