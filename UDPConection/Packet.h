#pragma once
#include <iostream>
#include <stdio.h>
#include <string>
#include <cstring>

using namespace std;

enum DataType
{
    MESSAGE,
    ACK
};


class Packet {
    // Socket s
    int seqNum;
    long unsigned int dataLength;
    DataType dataType;
    char* data;

    char* encaplulatedPacket;
    long unsigned int packetLength;
    public:       
        void innit(int SeqNum, char* Data, long unsigned int dataLength, DataType datatype);
        char* encaplulate();  
        void dencapsulate(char* data);
        void cleanup();

        int getSeqNum();
        long unsigned int getDataLength();
        DataType getDataType();
        char* getData();
        long unsigned int getPacketLength();

};