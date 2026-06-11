#include "DataTypes.h"

int FileIndicator::createNew(ID id, int fileSize, string path) {
     this->fileID = id;
     this->fileSize = fileSize;
     this->relativeFileLocation = path;
     return UUID_BYTE_SIZE + sizeof(int) + sizeof(int) + relativeFileLocation.length();
}

void FileIndicator::serialize(unsigned char* serializedData) {
    int offset = 0;

    // ID
    memcpy(serializedData+offset, this->fileID.getRaw(), UUID_BYTE_SIZE);
    offset += UUID_BYTE_SIZE;

    // FileSize
    memcpy(serializedData+offset, &this->fileSize, sizeof(this->fileSize));
    offset += sizeof(this->fileSize);

    // PathLength
    int len = this->relativeFileLocation.length();
    memcpy(serializedData+offset, &len, sizeof(len));
    offset += sizeof(len);

    // Path String
    memcpy(serializedData+offset, this->relativeFileLocation.data(), sizeof(len));
    offset += len;
}

FileIndicator FileIndicator::deserialize(unsigned char* serializedData) {
    FileIndicator newIndicator;
    int offset = 0;

    // ID
    newIndicator.fileID.set(serializedData+offset);
    offset += UUID_BYTE_SIZE;

    // File Size
    memcpy(&newIndicator.fileSize, serializedData+offset, sizeof(newIndicator.fileSize));
    offset += sizeof(newIndicator.fileSize);

    // path len
    int len;
    memcpy(&len, serializedData+offset, sizeof(len));
    offset += sizeof(len);

    // Path string
    string s(reinterpret_cast<char const*>(serializedData+offset), len);
    newIndicator.relativeFileLocation = s;

    return newIndicator;
}