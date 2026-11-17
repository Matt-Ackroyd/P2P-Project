#include "DataTypes.h"
#include "PrimaryClient.h"
#include "FileHandler.h"

FileContainer::FileContainer(std::string id, int byteLocation, unsigned char* data, int datalenth, DataTypes type) : 
Container(type, UUID_BYTE_SIZE + sizeof(int) + sizeof(int) + datalenth) {
    this->fileID = ID::clean(id);
    this->byteLocation = byteLocation;
    this->fileData = data;
    this->fileDataLen = datalenth;
    
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
    memcpy(data+offset, &this->fileDataLen, sizeof(this->fileDataLen));
    offset += sizeof(this->fileDataLen);

    // Data
    memcpy(data+offset, this->fileData, this->fileDataLen);
    offset += this->fileDataLen;
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
    int fileDataLen;
    memcpy(&fileDataLen, serializedata+offset, sizeof(fileDataLen));
    offset += sizeof(fileDataLen);

    // Data
    unsigned char* filedata = new unsigned char[fileDataLen];
    memcpy(filedata, serializedata+offset, fileDataLen);
    offset += fileDataLen;

    return FileContainer(fileID, bytelocation, filedata, fileDataLen, DataTypes::EMPTY);
}

std::string FileContainer::getFileID()
{
    return this->fileID;
}

int FileContainer::getByteLocation()
{
    return this->byteLocation;
}

unsigned char* FileContainer::getFileData() {
    return this->fileData;
}

int FileContainer::getFileDatalen() {
    return this->fileDataLen;
}



void FileContainer::onRequest(std::string serverID, std::string fileID, RemoteUser *requestee) { 
    PrimaryClient* client = PrimaryClient::getInstance();
    Server* server = client->getServer(serverID);
    if (server == NULL) {
        return;
    }

    if (!server->knownUsers.contains(*requestee->getID())) { // If they dont belong to this server dont send them anything
        return;
    }

    client->fileHandler->sendFile(fileID, requestee);

}