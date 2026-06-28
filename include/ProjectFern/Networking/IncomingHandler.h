#pragma once
#include <iostream>
#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <thread>
#include <deque>
#include <fstream>
#include <unordered_set>

#include "socketMacro.h"

#include "Packet.h"
#include "Encryption.h"
#include "UDPConnection.h"
#include "PrimaryClient.h"
#include "DataTypes.h"
#include "ML-KEM_Handshake.h"
#include "OutgoingHandler.h"

#define MAXLINE 3000

class IncomingHandler {
    public:     
        std::thread IncomingHandlerThread;

        IncomingHandler(int ReceivingPort);
        void acknowledgePacket(Packet packet, UDPConnection connectedUser);
    private:
        int nextExpectedSeqNum;
        // Bool to accept Incoming messages from other clients
        bool acceptIncoming;
        void incomingStartup(int ReceivingPort);
        void incomingLoop(SOCKTYPE socketfd, char* buffer, sockaddr_in cliaddr);
        std::deque<Packet> ReceivingBuffer;

        void handleAck(Packet packet, UDPConnection connectedUser);
        void handleConnectionRequest(Packet *packet, SOCKTYPE returnSock, sockaddr_in returnAdress, socklen_t returnLen);
        void handleConnectionResponse(Packet *packet, SOCKTYPE returnSock, sockaddr_in returnAdress, socklen_t returnLen);
        void handleRelayInfoResponse(Packet *packet, int datalen);
        void handlePacket(Packet *packet, int datalen, int temp);
        void handleKeepAlive(Packet *packet);

        void handleMessage(unsigned char* decryptedData);
};