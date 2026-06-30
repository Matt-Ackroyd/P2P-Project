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
     

    unsigned char* sharedSecret = NULL;

    
    int outgoingSeqNum = 1;
    
    std::deque<Packet*> outgoingBuffer;
    
public:    
    UDPConnection();
    ~UDPConnection();

    bool connected = false;

    unsigned char handshakeRandBuffer[ML_KEM_HANDSHAKE_RANDSIZE];
    
    void setAddr(char const *addr, int port);  
    void sendEncrypted(unsigned char* data, int datalen);
    void sendKeepAlive();
    void sendAck(int seqNum);
    void sendHandshakeRequest();
    unsigned char* getSharedSecret();
    void setSharedSecret(unsigned char* secret);
    void receivedAck(int seqNum);

    
    void addPacketToIncomingQueue(Packet* incomingPacket);
    int newSeqNum();
    int incomingSeqNum = 1;
    std::deque<Packet*> incommingBuffer; 
};