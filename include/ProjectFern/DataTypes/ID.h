#pragma once
#include <chrono>
#include <ctime>
#include <iostream>
#include <cstring>
#include <iomanip>
#include <openssl/rand.h>

#define UUID_BYTE_SIZE 16

//First 8 bytes is the timestamp in nanoseconds 
//Second set of 8 bytes is random
class ID {
private:
    std::string idString;
    unsigned char rawID[UUID_BYTE_SIZE];
    uint64_t timestamp = 0;
    void toString();
    unsigned char* toBytes();
    void GenerateNewID();
public:
    ID(std::string uuidString = "NULL");
    static ID fromBytes(unsigned char* existingUUID);
    std::string getString();
    unsigned char* getRaw();
    uint64_t getTimestamp();
};