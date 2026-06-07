#include "UUID.h"
//64bit timestamp in nanoseconds & 64bits of random

void UUID::set(unsigned char* existingUUID) {
     memcpy(this->ID.data(), existingUUID, UUID_BYTE_SIZE);
}

void UUID::GenerateNewID() {
    unsigned char id_buffer[UUID_BYTE_SIZE];
    // Grab Time stamp
    const auto p1 = std::chrono::system_clock::now();
    this->timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
                   p1.time_since_epoch()).count();
 
    // Copy Timestamp into UUID
    memcpy(id_buffer, &this->timestamp, 8);
    // Generate 64 Random Bits
    RAND_bytes(id_buffer+8, 8);

    //Generate String
    std::ostringstream stringS;
    stringS << std::hex << std::setfill('0');
    for (int i = 0; i < UUID_BYTE_SIZE; i++) {
        stringS << std::setw(2) << static_cast<int>(id_buffer[i]);
    }
    this->ID = stringS.str();
}

uint64_t UUID::getTimestamp() {
    // Guard clause for only calculating this value once
    if (this->timestamp != 0) {
        return this->timestamp;
    }

    // Converting first 8 Hex bytes to timestamp
    unsigned char temp_buffer[sizeof(this->timestamp)];
    for (int i = 0; i < sizeof(this->timestamp); i++) {
        // Grab the substring with the two hex values that represent this byte
        // i is multiplyed by two beacuse hex is represented by 2 values and our loop is counting up by one
        // so each incremint of the loop means we move over two in the substring
        std::string byteStr = ID.substr(i*2, 2);
        unsigned char byte = static_cast<unsigned char>(std::stoul(byteStr, nullptr, 16));
        temp_buffer[i] = byte;
    }
    // Save the timestamp value & return it
    memcpy(&this->timestamp, temp_buffer, sizeof(this->timestamp));
    return this->timestamp;
}

string UUID::get() {
    return this->ID;
}

unsigned char* UUID::getRaw() {
    return (unsigned char*)this->ID.data();
}