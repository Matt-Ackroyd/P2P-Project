#include "ID.h"
//64bit timestamp in nanoseconds & 64bits of random

ID::ID(std::string uuidString) {
    if (uuidString == "NULL") {
        this->GenerateNewID();
    } else {
        this->idString = uuidString;
        this->toBytes();
    }    
}

ID ID::fromBytes(unsigned char* existingUUID) {
    ID temp;
    memcpy(temp.rawID, existingUUID, UUID_BYTE_SIZE);
    temp.timestamp = 0;

    // Create New String
    temp.toString();
    return temp;
}

void ID::toString() {
    //Generate String
    std::ostringstream stringS;
    stringS << std::hex << std::setfill('0');
    for (int i = 0; i < UUID_BYTE_SIZE; i++) {
        stringS << std::setw(2) << static_cast<int>(rawID[i]);
    }
    this->idString = stringS.str();
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
        std::string byteStr = idString.substr(i*2, 2);
        unsigned char byte = static_cast<unsigned char>(std::stoul(byteStr, nullptr, 16));
        temp_buffer[i] = byte;
    }
    // Save the timestamp value & return it
    memcpy(&this->timestamp, temp_buffer, sizeof(this->timestamp));
    return this->timestamp;
}

std::string ID::getString() {
    return this->idString;
}

unsigned char* ID::getRaw() {
    return this->rawID;
}

unsigned char* ID::toBytes() {
  for (unsigned int i = 0; i < idString.length(); i += 2) {
    std::string byteString = idString.substr(i, 2);
    char byte = (char) strtol(byteString.c_str(), NULL, 16);
    this->rawID[i/2] = byte;
  }

  return rawID;
}