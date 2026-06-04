#include "UUID.h"
//64bit timestamp in nanoseconds & 64bits of random

void UUID::set(unsigned char* existingUUID) {
    memcpy(this->ID, existingUUID, UUID_BYTE_SIZE);
}

void UUID::GenerateNewID() {
    // Grab Time stamp
    const auto p1 = std::chrono::system_clock::now();
    const auto timeStamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
                   p1.time_since_epoch()).count();
 
    // Copy Timestamp into UUID
    memcpy(this->ID, &timeStamp, 8);
    // Generate 64 Random Bits
    RAND_bytes(this->ID+8, 8);
}

unsigned char* UUID::get() {
    return this->ID;
}