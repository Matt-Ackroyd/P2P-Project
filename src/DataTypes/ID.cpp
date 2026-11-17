#include "ID.h"
//64bit timestamp in nanoseconds & 64bits of random

std::string ID::stringFromBytes(unsigned char* rawID) {
    std::ostringstream stringS;
    stringS << std::hex << std::setfill('0');
    for (int i = 0; i < UUID_BYTE_SIZE; i++) {
        stringS << std::setw(2) << static_cast<int>(rawID[i]);
    }
    return stringS.str();
}

std::string ID::GenerateNewID() {
    // Grab Time stamp
    unsigned char rawID[UUID_BYTE_SIZE];

    const auto p1 = std::chrono::system_clock::now();
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
                   p1.time_since_epoch()).count();
 
    // Copy Timestamp into ID
    memcpy(rawID, &timestamp, 8);
    // Generate 64 Random Bits
    RAND_bytes(rawID+8, 8);

    // Create New String
    return stringFromBytes(rawID);
}

uint64_t ID::getTimestamp(std::string id) {
    uint64_t timestamp;

    // Converting first 8 Hex bytes to timestamp
    unsigned char bytes[UUID_BYTE_SIZE];
    BytesFromString(id, bytes);

    memcpy(&timestamp, bytes, 8);

    return timestamp;
}


void ID::BytesFromString(std::string id, unsigned char* buffer) {
    // If the string isn't correct size abort
    if (id.empty()) {
        memset(buffer, 0, UUID_BYTE_SIZE);
        return;
    }


    for (unsigned int i = 0; i < id.length(); i += 2) {
        std::string byteString = id.substr(i, 2);
        char byte = (char) strtol(byteString.c_str(), NULL, 16);
        buffer[i/2] = byte;
    }

}

std::string ID::clean(std::string input) {
    if (input.empty()) {
        return ID::GenerateNewID();
    } 
    return input;
}