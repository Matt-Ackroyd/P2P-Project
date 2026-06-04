#pragma once
#include <iostream>
#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <deque>
#include <fstream>

#include "Packet.h"
#include "Encryption.h"

using namespace std;

class UDPConnection {
private:
    

public:    
    UDPConnection(unsigned char* sharedSecret);
    ~UDPConnection();
    int SendingPort = 5000;
    int currentSeqNum; 
    int sock;
    void connectTo(char const *addr);  
    void send(unsigned char* data, int datalen);
    deque<Packet> sendingBuffer;

    unsigned char* sharedSecret;

    void sendConnectionRequest();

    unsigned char* getSharedSecret();
        
};