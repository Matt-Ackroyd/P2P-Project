#include "DataTypes.h"


int FileContainer::createNew(UUID id, int byteLocation, unsigned char* data, int datalenth) {
    this->fileID = id;
    this->byteLocation = byteLocation;
    this->data = data;
    this->datalen = datalen;
    return UUID_BYTE_SIZE + sizeof(int) + sizeof(int) + this->datalen;
}


void FileContainer::serialize(unsigned char* serializedData) {
    int offset = 0;

    // File ID
    memcpy(serializedData+offset, this->fileID.getRaw(), UUID_BYTE_SIZE);
    offset += UUID_BYTE_SIZE;

    // Byte Location
    memcpy(serializedData+offset, &this->byteLocation, sizeof(this->byteLocation));
    offset += sizeof(this->byteLocation);

    // Data Length
    memcpy(serializedData+offset, &this->datalen, sizeof(this->datalen));
    offset += sizeof(this->datalen);

    // Data
    memcpy(serializedData+offset, this->data, this->datalen);
    offset += this->datalen;
}

// Input a serialized array of data and get back an unserilized Filecontainer structure
FileContainer FileContainer::deserialize(unsigned char* serializedata) {
    FileContainer newFile;
    int offset = 0;

    // ID
    newFile.fileID.set(serializedata+offset);
    offset += UUID_BYTE_SIZE;

    // ByteLocation
    memcpy(&newFile.byteLocation, serializedata+offset, sizeof(newFile.byteLocation));
    offset += sizeof(newFile.byteLocation);

    // Datalen
    memcpy(&newFile.datalen, serializedata+offset, sizeof(newFile.datalen));
    offset += sizeof(newFile.datalen);

    // Data
    newFile.data[newFile.datalen];
    memcpy(newFile.data, serializedata+offset, newFile.datalen);
    offset += newFile.datalen;
    return newFile;
}