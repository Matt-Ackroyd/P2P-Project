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
    std::string ID;
    unsigned char rawID[UUID_BYTE_SIZE];
    uint64_t timestamp = 0;
    void toString();
public:
    void GenerateNewID();
    void set(unsigned char* existingUUID);
    std::string get();
    unsigned char* getRaw();
    uint64_t getTimestamp();
};