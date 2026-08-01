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
#include "RelayClient.h"
#include "Sync.h"

class IncomingHandler {
    public:     
        std::thread IncomingHandlerThread;

        static const int threadCount = 30;
        std::thread* ThreadManager[threadCount];
        std::mutex threadMTX;

        IncomingHandler(int ReceivingPort);
        void acknowledgePacket(Packet packet, UDPConnection connectedUser);
    private:
        int nextExpectedSeqNum;
        // Bool to accept Incoming messages from other clients
        bool acceptIncoming;
        void incomingStartup(int ReceivingPort);
        void IncomingLoop(char* buffer);
        void threadStarter(char *buffer, sockaddr_in cliaddr, int threadNumber);
        void onPacketRecived(char *buffer, sockaddr_in cliaddr);
        void handleIncoming(Packet *incomingPacket, sockaddr_in cliaddr);
        std::deque<Packet> ReceivingBuffer;

        void handleAck(Packet* packet);
        void handleConnectionRequest(Packet *packet);
        void handleConnectionResponse(Packet *packet);
        void handleRelayInfoResponse(Packet *packet);
        void handlePacket(Packet *packet);
        void handleKeepAlive(Packet *packet);

        void handleMessage(unsigned char* decryptedData);
        void handleNewServer(unsigned char *decryptedData);
        void handleNewTextChannel(unsigned char *decryptedData);
        void handleAddNewUserToServerRequest(unsigned char *decryptedData);
        void handleJoinRequest(unsigned char *decryptedData, RemoteUser *requestee);
        RemoteUser *onIncomingPacket(Packet *incomingPacket, sockaddr_in cliaddr);
};