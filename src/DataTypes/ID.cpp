#include "ID.h"
//64bit timestamp in nanoseconds & 64bits of random

void ID::set(unsigned char* existingUUID) {
    memcpy(this->rawID, existingUUID, UUID_BYTE_SIZE);
    this->timestamp = 0;

    // Create New String
    this->toString();
}

void ID::toString() {
    //Generate String
    std::ostringstream stringS;
    stringS << std::hex << std::setfill('0');
    for (int i = 0; i < UUID_BYTE_SIZE; i++) {
        stringS << std::setw(2) << static_cast<int>(rawID[i]);
    }
    this->ID = stringS.str();
}

void ID::GenerateNewID() {
    // Grab Time stamp
    const auto p1 = std::chrono::system_clock::now();
    this->timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
                   p1.time_since_epoch()).count();
 
    // Copy Timestamp into ID
    memcpy(rawID, &this->timestamp, 8);
    // Generate 64 Random Bits
    RAND_bytes(rawID+8, 8);

    // Create New String
    this->toString();
}

uint64_t ID::getTimestamp() {
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

string ID::get() {
    return this->ID;
}

unsigned char* ID::getRaw() {
    return this->rawID;
}