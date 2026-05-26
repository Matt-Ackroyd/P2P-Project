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
#include "DataTypes/DataTypes.h"

#define MAXLINE 1000
#define KEYLENGTH 1568

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
        deque<Packet> ReceivingBuffer;

        void handleAck(Packet packet, User connectedUser);
        void handleConnectionRequest(Packet packet);
        void handleConnectionResponse(Packet packet);
        void handlePacket(Packet packet, User connectedUser);
};