#include "DataTypes.h"


FileContainer::FileContainer(std::string id, int byteLocation, unsigned char* data, int datalenth, DataTypes type) : 
Container(type, UUID_BYTE_SIZE + sizeof(int) + sizeof(int) + datalenth) {
    this->fileID = ID::clean(id);
    this->byteLocation = byteLocation;
    this->fileData = data;
    this->datalen = datalen;
    
    if (type == FILETYPE) {
        serialize();
    }
}

FileContainer::~FileContainer() {
    delete[] fileData;
}


void FileContainer::serialize() {
    // File ID
    unsigned char uuid[UUID_BYTE_SIZE];
    ID::BytesFromString(this->fileID, uuid);
    memcpy(data+offset, uuid, UUID_BYTE_SIZE);
    offset += UUID_BYTE_SIZE;

    // Byte Location
    memcpy(data+offset, &this->byteLocation, sizeof(this->byteLocation));
    offset += sizeof(this->byteLocation);

    // Data Length
    memcpy(data+offset, &this->datalen, sizeof(this->datalen));
    offset += sizeof(this->datalen);

    // Data
    memcpy(data+offset, this->fileData, this->datalen);
    offset += this->datalen;
}

// Input a serialized array of data and get back an unserilized Filecontainer structure
FileContainer FileContainer::deserialize(unsigned char* serializedata) {
    int offset = 4;

    // ID
    std::string fileID = ID::stringFromBytes(serializedata+offset);
    offset += UUID_BYTE_SIZE;

    // ByteLocation
    int bytelocation;
    memcpy(&bytelocation, serializedata+offset, sizeof(bytelocation));
    offset += sizeof(bytelocation);

    // Datalen
    int datalen;
    memcpy(&datalen, serializedata+offset, sizeof(datalen));
    offset += sizeof(datalen);

    // Data
    unsigned char* filedata = new unsigned char[datalen];
    memcpy(filedata, serializedata+offset, datalen);
    offset += datalen;

    return FileContainer(fileID, bytelocation, filedata, datalen, DataTypes::EMPTY);
}