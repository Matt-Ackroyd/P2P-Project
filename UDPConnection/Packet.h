#pragma once
#include <iostream>
#include <stdio.h>
#include <string>
#include <cstring>

using namespace std;

enum DataType
{
    ACK,
    MESSAGE,
    LARGEFILE // Rename??
};


class Packet {
    // Socket s
    int seqNum;
    int senderID;
    int serverID;
    int channelID;
    long unsigned int dataLength;
    DataType dataType;
    char* data;

    char* encaplulatedPacket;
    long unsigned int packetLength;
    public:       
        void innit(char* Data, long unsigned int dataLength, DataType datatype);
        char* encaplulate();  
        void dencapsulate(char* data);
        void cleanupAfterSend();
        void cleanupAfterReceive ();

        int getSeqNum();
        void setSeqNum(int SeqNum);
        long unsigned int getDataLength();
        DataType getDataType();
        char* getData();
        long unsigned int getPacketLength();

};