#include "DataTypes.h"

FileHostClaim::FileHostClaim(std::string fileID, DataTypes type) : Container(type, UUID_BYTE_SIZE) {
    this->fileID = fileID;

    if (type == DataTypes::FILE_HOST_CLAIM) {
        ID::BytesFromString(this->fileID, this->data);
    }
}

std::string FileHostClaim::deserialize(unsigned char* serializedData) {
    // The dataType has been removed by now
    int offset = sizeof(DataTypes);

    // ServerID
    std::string fileID = ID::stringFromBytes(serializedData+offset);
    offset += UUID_BYTE_SIZE;

    return fileID;
}


void FileHostClaim::onAddRequest() {}
void FileHostClaim::onRemoveRequest() {}