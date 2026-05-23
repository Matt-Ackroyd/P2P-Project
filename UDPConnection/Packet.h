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
    CONNECTION_REQUEST
};


class Packet {
private:
    PacketType packetType;
    int seqNum;
    int senderID;
    long unsigned int dataLength;
    char* data;

    char* encaplulatedPacket;
    long unsigned int packetLength;
public:       
    void innit(char* Data, long unsigned int dataLength, PacketType PacketType);
    char* encaplulate();  
    void dencapsulate(char* data);
    void cleanupAfterSend();
    void cleanupAfterReceive();

    void encrypt(const unsigned char* plaintext, const unsigned char* key);
    void decrypt(const unsigned char* key);

    int getSeqNum();
    void setSeqNum(int SeqNum);
    long unsigned int getDataLength();
    PacketType getPacketType();
    char* getData();
    long unsigned int getPacketLength();

};