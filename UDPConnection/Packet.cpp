#include "Packet.h"


void Packet::innit(char* Data, long unsigned int DataLength, PacketType packetType) {
    this->packetType = packetType;
    this->data = Data;
    this->dataLength = DataLength;
}

char* Packet::encaplulate() {
    this->packetLength = sizeof(this->seqNum) + sizeof(this->dataLength) + sizeof(this->packetType) + this->dataLength;
    this->encaplulatedPacket = new char[this->packetLength];    

    // TODO add safty checks
    // Add SeqNum to Output 
    memcpy(this->encaplulatedPacket, &this->seqNum, sizeof(this->seqNum));

    // Add Data Length to Output
    long unsigned int offset = sizeof(this->seqNum);
    memcpy(this->encaplulatedPacket+offset, &this->dataLength, sizeof(this->dataLength));

    // Add Data Type to Output
    offset += sizeof(this->dataLength);
    memcpy(this->encaplulatedPacket+offset, &this->packetType, sizeof(this->packetType));

    // Add Data to Output
    offset += sizeof(this->packetType);
    memcpy(this->encaplulatedPacket+offset, this->data, this->dataLength);

    return this->encaplulatedPacket;
}

void Packet::dencapsulate(char* recivedPacket) {
    //TODO Safty Checks
    long unsigned int offset = 0;
    memcpy(&this->seqNum, recivedPacket+offset, sizeof(this->seqNum));

    offset += sizeof(this->seqNum);
    memcpy(&this->dataLength, recivedPacket+offset, sizeof(this->dataLength));

    offset += sizeof(this->dataLength);
    memcpy(&this->packetType, recivedPacket+offset, sizeof(this->packetType));

    offset += sizeof(this->packetType);
    this->data = new char[dataLength];
    memcpy(this->data, recivedPacket+offset, this->dataLength);

    

}

int Packet::getSeqNum() {
    return this->seqNum;
}
void Packet::setSeqNum(int SeqNum) {
    this->seqNum = SeqNum;
}
long unsigned int Packet::getDataLength() {
    return this->dataLength;
}
PacketType Packet::getPacketType() {
    return this->packetType;
}
char* Packet::getData() {
    return this->data;
}
long unsigned int Packet::getPacketLength() {
    return this->packetLength;
}


void Packet::cleanupAfterSend() {
    delete this->encaplulatedPacket;
}
void Packet::cleanupAfterReceive() {
    delete this->data;
}




