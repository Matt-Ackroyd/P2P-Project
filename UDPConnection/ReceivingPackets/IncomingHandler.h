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
#include "Encryption/Encryption.h"
#include "UDPConnection.h"
#include "../../Users/Client.h"

#define MAXLINE 1000

using namespace std;

class IncomingHandler {
    public:     
        thread recvThread;
        void acknowledgePacket(Packet packet, UDPConnection connectedUser);
        void enableIncomingTraffic(int ReceivingPort);
    private:
        int nextExpectedSeqNum;
        // Bool to accept Incoming messages from other clients
        bool acceptIncoming;
        void startReceiving(int ReceivingPort);
        void handlePacket(Packet packet);
        deque<Packet> ReceivingBuffer;

        void handleAck(Packet packet);
        void handleConnectionRequest(Packet packet);
        void handleConnectionResponse(Packet packet);
        void handleRegularPacket(Packet packet);
};