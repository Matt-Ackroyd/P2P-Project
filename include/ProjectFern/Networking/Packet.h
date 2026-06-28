#pragma once
#include <iostream>
#include <stdio.h>
#include <string>
#include <cstring>

#include "Encryption.h"
#include "ID.h"
//#include "PrimaryClient.h"

enum PacketType
{
    PACKET,
    ACK,
    HANDSHAKE_REQUEST,
    HANDSHAKE_RESPONSE,
    CONNECTION_REQUEST,
    RELAY_REQUEST_USER_REGISTRATION,
    RELAY_REQUEST_UPDATE_CONNECTION_INFO,
    RELAY_USER_INFO,
    HELLO,
    HELLO_REPLY,
    KEEP_ALIVE,
    NONE
};


class Packet {
private:
    PacketType packetType;
    int seqNum;
    unsigned char IV[AES_256_IV_LENGTH];
    unsigned char MAC[AES_256_GCM_TAG_LENGTH];
    char* data;

public:       
    ID packetAuthorID;

    Packet(int seqNum, PacketType packetType, ID* author);
    ~Packet();

    int serialize(char* unserializedData, int dataLen, unsigned char* IV, unsigned char* MAC);  
    int deserialize(char* serializedData);

    int getSeqNum();
    PacketType getPacketType();
    char* getData();
    unsigned char* getIV();
    unsigned char* getTag();

    // PacketType(4) + SeqNum(4) + UUID(16) + dataLen(4) + ControlVar(1)
    static int const MIN_PACKET_SIZE = sizeof(packetType) + sizeof(seqNum) + UUID_BYTE_SIZE + sizeof(int) + sizeof(char);
};