#include "Packet.h"


Packet::Packet(int seqNum, PacketType packetType, std::string* author) {
    if (author != NULL) {
        this->packetAuthorID = *author;
    }
    
    this->seqNum = seqNum;
    this->packetType = packetType;
    //this->data = new unsigned char[10];
}


int Packet::serialize(char* unserializedData, int dataLen, unsigned char* IV, unsigned char* MAC) {
    this->dataLen = dataLen;
    unsigned char controlVar = (char)0;
    this->packetLength = Packet::MIN_PACKET_SIZE + dataLen;

    // Control variable to let the reciver know to expect the IV(1), MAC(2) or both(3)
    if (IV != NULL) {
        this->packetLength += AES_256_IV_LENGTH;
        controlVar += (char)1;
    }
    if (MAC != NULL) {
        this->packetLength += AES_256_GCM_TAG_LENGTH;
        controlVar += (char)2;
    }

    this->data = new char[packetLength]; 
    memset(this->data, 0, packetLength);
    
    long unsigned int offset = 0;

    // Add Packet Type to Output
    memcpy(this->data+offset, &this->packetType, sizeof(this->packetType));
    offset += sizeof(this->packetType);

    // Add SeqNum to Output 
    memcpy(this->data+offset, &this->seqNum, sizeof(this->seqNum));
    offset += sizeof(this->seqNum);

    // Sender Id
    unsigned char uuid[UUID_BYTE_SIZE];
    ID::BytesFromString(this->packetAuthorID, uuid);
    memcpy(this->data+offset, uuid, UUID_BYTE_SIZE);
    offset += UUID_BYTE_SIZE;

    // Add Data Length to Output
    memcpy(this->data+offset, &dataLen, sizeof(dataLen));
    offset += sizeof(dataLen);

    // Add Data to Output
    memcpy(this->data+offset, unserializedData, dataLen);
    offset += dataLen;

    // Iv/Mac Control val
    memcpy(this->data+offset, &controlVar, sizeof(controlVar));
    offset += sizeof(controlVar);

    // IV
    if (IV != NULL) {
        memcpy(this->data+offset, IV, AES_256_IV_LENGTH);
        offset += AES_256_IV_LENGTH;
    }

    // MAC
    if (MAC != NULL) {
        memcpy(this->data+offset, MAC, AES_256_GCM_TAG_LENGTH);
    }

    return this->packetLength;
}

// Returns data length
int Packet::deserialize(char* serializedData) {
    long unsigned int offset = 0;

    // PacketType
    memcpy(&this->packetType, serializedData+offset, sizeof(this->packetType));
    offset += sizeof(this->packetType);

    // Seqence Number
    memcpy(&this->seqNum, serializedData+offset, sizeof(this->seqNum));
    offset += sizeof(this->seqNum);

    // Sender Id
    this->packetAuthorID = ID::stringFromBytes((unsigned char*)serializedData+offset);
    offset += UUID_BYTE_SIZE;

    // DataLength
    memcpy(&this->dataLen, serializedData+offset, sizeof(this->dataLen));
    offset += sizeof(this->dataLen);

    if (0 > this->dataLen || this->dataLen > 3000) {
        this->data = new char[1];
        throw std::runtime_error("Bad Packet Buffer\n");
    }
    
    // Data
    this->data = new char[this->dataLen];
    memcpy(this->data, serializedData+offset, this->dataLen);
    offset += dataLen;

    // Iv/Mac Control val
    unsigned char controlVal;
    memcpy(&controlVal, serializedData+offset, sizeof(controlVal));
    offset += sizeof(controlVal);

    // IV (only if it was sent)
    this->IV == NULL;
    if (controlVal == 1 || controlVal == 3) {
        memcpy(this->IV, serializedData+offset, AES_256_IV_LENGTH);
        offset += AES_256_IV_LENGTH;
    }

    // MAC (only if it was sent)
    this->MAC == NULL;
    if (controlVal == 2 || controlVal == 3) {
        memcpy(this->MAC, serializedData+offset, AES_256_GCM_TAG_LENGTH);
    }

    return dataLen;
}

int Packet::getSeqNum() {
    return this->seqNum;
}
PacketType Packet::getPacketType() {
    return this->packetType;
}
char* Packet::getData() {
    return this->data;
}
unsigned char* Packet::getTag() {
    return this->MAC;
}
unsigned char* Packet::getIV() {
    return this->IV;
}
int Packet::getDataLength() {
    return this->dataLen;
}
int Packet::getPacketlength() {
    return this->packetLength;
}


Packet::~Packet() {
    delete[] this->data;
}




