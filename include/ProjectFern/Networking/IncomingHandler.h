#pragma once
#include <iostream>
#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <thread>
#include <deque>
#include <fstream>

#include "socketMacro.h"

#include "Packet.h"
#include "Encryption.h"
#include "UDPConnection.h"
#include "PrimaryClient.h"
#include "DataTypes.h"

#define MAXLINE 3000

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

        void handleAck(Packet packet, UDPConnection connectedUser);
        void handleConnectionRequest(Packet *packet, int returnSock, sockaddr_in *returnAdress, socklen_t returnLen);
        void handleConnectionResponse(Packet *packet, int returnSock, sockaddr_in *returnAdress, socklen_t returnLen);
        void handlePacket(Packet *packet, int datalen, int temp);
};