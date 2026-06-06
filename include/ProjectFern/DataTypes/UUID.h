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
class UUID {
private:
    string ID;
public:
    void GenerateNewID();
    void set(unsigned char* existingUUID);
    string get();
    unsigned char* get_raw();
};