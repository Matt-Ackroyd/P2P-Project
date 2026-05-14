#pragma once
#include <iostream>
#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>

#include "Packet.h"

#define MAXLINE 1000

using namespace std;

class UDPConnection {
    // Socket s
    public:       
        int SendingPort = 5000;
        void connectTo(char const *addr);  
        void send(char const *message);
        void static startReceiving(int ReceivingPort);
    private:
        int s;
        // Bool to accept Incoming messages from other clients
        bool acceptIncoming;
        
        
        
};