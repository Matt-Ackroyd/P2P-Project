#pragma once
#include <chrono>
#include <ctime>
#include <iostream>
#include <cstring>
#include <iomanip>
#include <openssl/rand.h>

#define UUID_BYTE_SIZE 16

using namespace std;

//First 8 bytes is the timestamp in nanoseconds 
//Second set of 8 bytes is random
class ID {
private:
    string ID;
    unsigned char rawID[UUID_BYTE_SIZE];
    uint64_t timestamp = 0;
    void toString();
public:
    void GenerateNewID();
    void set(unsigned char* existingUUID);
    string get();
    unsigned char* getRaw();
    uint64_t getTimestamp();
};