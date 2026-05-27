#include "Packet.h"


Packet::Packet(int seqNum, PacketType packetType) {
    this->seqNum = seqNum;
    this->packetType = packetType;

}


unsigned char* Packet::serialize(unsigned char* unserializedData, int dataLen, unsigned char* IV, unsigned char* MAC) {
    size_t packetLength = sizeof(this->packetType) + sizeof(this->seqNum) + sizeof(dataLen) + AES_256_IV_LENGTH + dataLen + AES_256_GCM_TAG_LENGTH;
    this->data = new unsigned char[packetLength];    

    // Add Packet Type to Output
    long unsigned int offset = 0;
    memcpy(this->data+offset, &this->packetType, sizeof(this->packetType));

    // Add SeqNum to Output 
    offset += sizeof(this->packetType);
    memcpy(this->data+offset, &this->seqNum, sizeof(this->seqNum));

    // Add Data Length to Output
    offset += sizeof(this->seqNum);
    memcpy(this->data+offset, &dataLen, sizeof(dataLen));

    // IV
    offset += sizeof(dataLen);
    memcpy(this->data+offset, IV, AES_256_IV_LENGTH);

    // Add Data to Output
    offset += AES_256_IV_LENGTH;
    memcpy(this->data+offset, unserializedData, dataLen);

    // MAC
    offset += dataLen;
    memcpy(this->data+offset, MAC, AES_256_GCM_TAG_LENGTH);

    return this->data;
}

// Returns data length
int Packet::deserialize(unsigned char* serializedData) {
    int dataLen;

    // PacketType
    long unsigned int offset = 0;
    memcpy(&this->packetType, serializedData+offset, sizeof(this->packetType));

    // Seqence Number
    offset += sizeof(this->packetType);
    memcpy(&this->seqNum, serializedData+offset, sizeof(this->seqNum));

    // DataLength
    offset += sizeof(this->seqNum);
    memcpy(&dataLen, serializedData+offset, sizeof(dataLen));

    // IV
    offset += sizeof(dataLen);
    memcpy(this->IV, serializedData+offset, AES_256_IV_LENGTH);

    // Data
    offset += AES_256_IV_LENGTH;
    this->data = new unsigned char[dataLen];
    memcpy(this->data, serializedData+offset, dataLen);

    // MAC
    offset += dataLen;
    memcpy(this->MAC, serializedData+offset, AES_256_GCM_TAG_LENGTH);

    return dataLen;

    
    
    

    

}

int Packet::getSeqNum() {
    return this->seqNum;
}
PacketType Packet::getPacketType() {
    return this->packetType;
}
unsigned char* Packet::getData() {
    return this->data;
}



Packet::~Packet() {
    delete this->data;
}




