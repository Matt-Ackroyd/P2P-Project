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
#include <mutex>
#include <unordered_map>
#include <unordered_set>

#include "Packet.h"
#include "Encryption.h"
#include "Sync.h"

class Server;
class TextChannel;
class RemoteUser;

class UDPConnection {
private:
    struct sockaddr_in connectionAddr;
    int sock;
    unsigned char* sharedSecret = NULL;
    int outgoingSeqNum = 1; // Next packet number we should send to them
    
    
public:    
    UDPConnection();
    ~UDPConnection();

    EVP_PKEY* DSAkey = nullptr;

    std::chrono::time_point<std::chrono::_V2::system_clock, std::chrono::_V2::system_clock::duration> lastHeardFrom = std::chrono::system_clock::now();
    std::chrono::time_point<std::chrono::_V2::system_clock, std::chrono::_V2::system_clock::duration> sendCooldown = std::chrono::system_clock::now();
    bool connected = false;
    bool requestHandshakeOnceConnected = false;
    // TODO Have a better Buffer system (right now it only stores a server join request for after a connection is established)
    std::string bufferedServerInvitation = "";

    unsigned char handshakeRandBuffer[ML_KEM_HANDSHAKE_RANDSIZE];
    
    void setAddr(char const *addr, int port);
    void sendAck(std::string packetID);
    void sendEncrypted(unsigned char *data, int datalen);
    void sendKeepAlive();
    void sendHello();
    void sendPacket(Packet *packet);
    void sendHandshakeRequest();
    unsigned char* getSharedSecret();
    void setSharedSecret(unsigned char* secret);
    void receivedAck(std::string packetID);

    void addPacketToOutgoingQueue(Packet* outgoingPacket);
    void addPacketToIncomingQueue(Packet* incomingPacket);
    void removePacketFromIncomingQueue(std::string packetID);
    int newSeqNum();
    void sendServer(Server *server);
    void sendTextChannel(TextChannel *channel);
    void sendJoinRequest(std::string invitationCode);
    void sendAddUserToServerRequest(RemoteUser *user, Server *server);
    void resetConnection();

    void sendSyncRequest(Server *server);

    std::mutex incomingmtx;
    std::mutex outgoingmtx;

    std::unordered_map<std::string, Packet*> outgoingBuffer;
    std::unordered_map<std::string, std::chrono::time_point<std::chrono::_V2::system_clock, std::chrono::_V2::system_clock::duration>> incommingBuffer; 

    int windowSize = 700;
    //int numOfOutgoingPackets = 0;
    //int posOfFirstOutgoingPacket = 1;

    std::unordered_map<std::string, SyncRequest*> syncRequests;
    std::unordered_set<std::string> listOfRequestedIDs;
    bool synced = false;

};