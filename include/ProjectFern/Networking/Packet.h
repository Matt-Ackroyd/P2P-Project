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
    RELAY_REGISTER_REQUEST,
    RELAY_HANDSHAKE_RESPONSE,
};


class Packet {
private:
    PacketType packetType;
    int seqNum;
    unsigned char IV[AES_256_IV_LENGTH];
    unsigned char MAC[AES_256_GCM_TAG_LENGTH];
    char* data;
    int dataLen;
    int packetLength;


    std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> lastSent;

public:       
    std::string packetAuthorID;

    Packet(int seqNum, PacketType packetType, std::string* author);
    ~Packet();

    int serialize(char* unserializedData, int dataLen, unsigned char* IV, unsigned char* MAC);  
    int deserialize(char* serializedData);

    int getSeqNum();
    PacketType getPacketType();
    char* getData();
    int getDataLength();
    int getPacketlength();
    unsigned char* getIV();
    unsigned char* getTag();

    // PacketType(4) + SeqNum(4) + UUID(16) + dataLen(4) + ControlVar(1)
    static int const MIN_PACKET_SIZE = sizeof(packetType) + sizeof(seqNum) + UUID_BYTE_SIZE + sizeof(int) + sizeof(char);
};