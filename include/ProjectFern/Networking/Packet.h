#pragma once
#include <iostream>
#include <stdio.h>
#include <string>
#include <cstring>
#include <chrono>

#include "socketMacro.h"
#include "Encryption.h"
#include "ID.h"

#define MAX_RESEND_AMOUNT 10
#define MAXLINE 8192

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
    HELLO
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
public:       
    std::string packetAuthorID;
    // Marks the next time where this packet should be sent
    std::chrono::time_point<std::chrono::_V2::system_clock, std::chrono::_V2::system_clock::duration> timeToSend = std::chrono::system_clock::now();
    int timesResent = 0;

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