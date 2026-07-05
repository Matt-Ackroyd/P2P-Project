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
public:
    ID() = delete;

    static std::string stringFromBytes(unsigned char* existingUUID);
    static void BytesFromString(std::string id, unsigned char* buffer); 
    static std::string GenerateNewID();
    static uint64_t getTimestamp(std::string id);

    // returns a new ID if the input is empty or just returns the input if its not empty
    static std::string clean(std::string input);
};