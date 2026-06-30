#pragma once
#include <iostream>
#include <stdio.h>
#include <string>
#include <cstring>
#include <chrono>

#include "socketMacro.h"
#include "Encryption.h"
#include "ID.h"

enum PacketType
{
    NONE,
    ACK,
    KEEP_ALIVE,
    RELAY_USER_INFO,
    // Requires acknowledgement 
    PACKET,
    HANDSHAKE_REQUEST,
    HANDSHAKE_RESPONSE,
    CONNECTION_REQUEST,
    RELAY_REQUEST_USER_REGISTRATION,
    RELAY_REQUEST_UPDATE_CONNECTION_INFO,
};


class Packet {
private:
    PacketType packetType;
    int seqNum;
    unsigned char IV[AES_256_IV_LENGTH];
    unsigned char MAC[AES_256_GCM_TAG_LENGTH];
    char* data;
    int dataLen;


    std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> lastSent;

public:       
    ID packetAuthorID;
    sockaddr_in returnAddr;

    Packet(int seqNum, PacketType packetType, ID* author);
    ~Packet();

    int serialize(char* unserializedData, int dataLen, unsigned char* IV, unsigned char* MAC);  
    int deserialize(char* serializedData);

    int getSeqNum();
    PacketType getPacketType();
    char* getData();
    int getDataLength();
    unsigned char* getIV();
    unsigned char* getTag();

    // PacketType(4) + SeqNum(4) + UUID(16) + dataLen(4) + ControlVar(1)
    static int const MIN_PACKET_SIZE = sizeof(packetType) + sizeof(seqNum) + UUID_BYTE_SIZE + sizeof(int) + sizeof(char);
};