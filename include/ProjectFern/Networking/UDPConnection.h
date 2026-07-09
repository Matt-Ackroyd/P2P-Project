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

class Server;
class TextChannel;
class RemoteUser;

class UDPConnection {
private:
    struct sockaddr_in connectionAddr;
    int sock;
    unsigned char* sharedSecret = NULL;
    int outgoingSeqNum = 1;
    
    
public:    
    UDPConnection();
    ~UDPConnection();

    std::chrono::time_point<std::chrono::_V2::system_clock, std::chrono::_V2::system_clock::duration> lastHeardFrom = std::chrono::system_clock::now();
    std::chrono::time_point<std::chrono::_V2::system_clock, std::chrono::_V2::system_clock::duration> sendCooldown = std::chrono::system_clock::now();
    bool connected = false;
    bool requestHandshakeOnceConnected = false;
    // TODO Have a better Buffer system (right now it only stores a server join request for after a connection is established)
    std::string bufferedServerInvitation = "";

    unsigned char handshakeRandBuffer[ML_KEM_HANDSHAKE_RANDSIZE];
    
    void setAddr(char const *addr, int port);  
    void sendEncrypted(unsigned char* data, int datalen);
    void sendKeepAlive();
    void sendAck(int seqNum);
    void sendHello();
    void sendPacket(Packet *packet);
    void sendHandshakeRequest();
    unsigned char* getSharedSecret();
    void setSharedSecret(unsigned char* secret);
    void receivedAck(int seqNum);

    void addPacketToOutgoingQueue(Packet* outgoingPacket);
    void addPacketToIncomingQueue(Packet* incomingPacket);
    int newSeqNum();
    void sendServer(Server *server);
    void sendTextChannel(TextChannel *channel);
    void sendJoinRequest(std::string invitationCode);
    void sendAddUserToServerRequest(RemoteUser *user, Server *server);
    void resetConnection();

    int incomingSeqNum = 1;
    std::deque<Packet*> outgoingBuffer;
    std::deque<Packet*> incommingBuffer; 
};