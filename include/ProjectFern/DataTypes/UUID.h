#pragma once
#include <chrono>
#include <ctime>
#include <iostream>
#include <cstring>
#include <openssl/rand.h>

#define UUID_BYTE_SIZE 16

using namespace std;

//First 8 bytes is the timestamp in nanoseconds 
//Second set of 8 bytes is random
class UUID {
private:
    unsigned char ID[UUID_BYTE_SIZE];
public:
    void GenerateNewID();
    void set(unsigned char* existingUUID);
    unsigned char* get();
};