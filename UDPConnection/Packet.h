#pragma once
#include <iostream>
#include <stdio.h>
#include <string>
#include <cstring>

#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

using namespace std;



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
    int senderID;
    unsigned char* IV;
    unsigned char* MAC;
    long unsigned int dataLength;
    unsigned char* data;

    unsigned char* encaplulatedPacket;
    long unsigned int packetLength;
public:       
    void innit(unsigned char* Data, long unsigned int dataLength, PacketType PacketType);
    unsigned char* encaplulate();  
    void dencapsulate(unsigned char* data);
    void cleanupAfterSend();
    void cleanupAfterReceive();

    int getSeqNum();
    void setSeqNum(int SeqNum);
    long unsigned int getDataLength();
    PacketType getPacketType();
    unsigned char* getData();
    long unsigned int getPacketLength();

};