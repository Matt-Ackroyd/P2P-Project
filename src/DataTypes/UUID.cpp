#include "UUID.h"
//64bit timestamp in nanoseconds & 64bits of random


UUID::UUID(__int128_t existingID) {
    this->ID = existingID;
    if (this->ID == NULL) {
        this->ID = this->GenerateNewID();
    }
}

__int128_t UUID::GenerateNewID() {
    // Grab Time stamp
    const auto p1 = std::chrono::system_clock::now();
    this->timeStamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
                   p1.time_since_epoch()).count();
 
    std::cout << "seconds since epoch: "
              << timeStamp << '\n';

    // Generate 64 Random Bits
    unsigned char rand[8];
    RAND_bytes(rand, 8);

    // Copy them into a 128 bit ID
    __uint128_t newID = (long long)64324234234234;
    memcpy(&newID, &timeStamp, 8);
    memcpy(&newID+8, rand, 8);

    return newID;
}