#pragma once
#include <chrono>
#include <ctime>
#include <iostream>
#include <cstring>
#include <openssl/rand.h>


using namespace std;

class UUID {
private:
    __int128_t ID;
    int64_t timeStamp;

    __int128_t GenerateNewID();
    

public:
    UUID(__int128_t existingID);

    
};