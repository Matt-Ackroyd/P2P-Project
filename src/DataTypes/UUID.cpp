#include "UUID.h"
//64bit timestamp in nanoseconds & 64bits of random

void UUID::set(unsigned char* existingUUID) {
     memcpy(this->ID.data(), existingUUID, UUID_BYTE_SIZE);
}

void UUID::GenerateNewID() {
    unsigned char id_buffer[UUID_BYTE_SIZE];
    // Grab Time stamp
    const auto p1 = std::chrono::system_clock::now();
    const auto timeStamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
                   p1.time_since_epoch()).count();
 
    // Copy Timestamp into UUID
    memcpy(id_buffer, &timeStamp, 8);
    // Generate 64 Random Bits
    RAND_bytes(id_buffer+8, 8);

    //Generate String
    std::ostringstream stringS;
    stringS << std::hex << std::setfill('0');
    for (int i = 0; i < UUID_BYTE_SIZE; i++) {
        stringS << std::setw(2) << static_cast<int>(id_buffer[i]);
    }
    cout << stringS.str() << "\n";
    this->ID = stringS.str();
}

string UUID::get() {
    return this->ID;
}

unsigned char* UUID::get_raw() {
    return (unsigned char*)this->ID.data();
}