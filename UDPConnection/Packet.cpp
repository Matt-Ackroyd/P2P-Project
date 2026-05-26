#include "Packet.h"


Packet::Packet(int seqNum, PacketType packetType) {
    this->seqNum = seqNum;
    this->packetType = packetType;

}


unsigned char* Packet::serialize(unsigned char* unserializedData, int dataLen, unsigned char* IV, unsigned char* MAC) {
    size_t packetLength = sizeof(this->packetType) + sizeof(this->seqNum) + sizeof(dataLen) + IVSIZE + dataLen + MACSIZE;
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
    memcpy(this->data+offset, IV, IVSIZE);

    // Add Data to Output
    offset += IVSIZE;
    memcpy(this->data+offset, unserializedData, dataLen);

    // MAC
    offset += dataLen;
    memcpy(this->data+offset, IV, IVSIZE);

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
    memcpy(this->IV, serializedData+offset, IVSIZE);

    // Data
    offset += IVSIZE;
    this->data = new unsigned char[dataLen];
    memcpy(this->data, serializedData+offset, dataLen);

    // MAC
    offset += dataLen;
    memcpy(this->MAC, serializedData+offset, MACSIZE);

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




