#pragma once
#include <iostream>
#include <stdio.h>
#include <string>
#include <cstring>

#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

using namespace std;

#define MACSIZE 10
#define IVSIZE 10

enum PacketType
{
    PACKET,
    ACK,
    CONNECTION_REQUEST,
    CONNECTION_RESPONSE
};


class Packet {
private:
    PacketType packetType;
    int seqNum;
    unsigned char IV[IVSIZE];
    unsigned char MAC[MACSIZE];
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