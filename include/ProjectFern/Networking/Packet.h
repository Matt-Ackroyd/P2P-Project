#pragma once
#include <iostream>
#include <stdio.h>
#include <string>
#include <cstring>

#include "Encryption.h"

using namespace std;

enum PacketType
{
    PACKET,
    ACK,
    CONNECTION_REQUEST,
    CONNECTION_RESPONSE,
    NONE
};


class Packet {
private:
    PacketType packetType;
    int seqNum;
    unsigned char IV[AES_256_IV_LENGTH];
    unsigned char MAC[AES_256_GCM_TAG_LENGTH];
    unsigned char* data;

public:       
    int senderID;

    Packet(int seqNum, PacketType packetType);
    ~Packet();

    unsigned char* serialize(unsigned char* unserializedData, int dataLen, unsigned char* IV, unsigned char* MAC);  
    int deserialize(unsigned char* serializedData);

    int getSeqNum();
    PacketType getPacketType();
    unsigned char* getData();
};