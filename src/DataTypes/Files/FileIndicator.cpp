#include "DataTypes.h"
#include "PrimaryClient.h"
#include "DatabaseConnection.h"

FileIndicator::FileIndicator(std::string relativePath, int fileSize, std::string serverID, unsigned char* signature, std::string localPath = "", 
     std::string id, DataTypes type) : Container(type, UUID_BYTE_SIZE*2 + sizeof(int) + sizeof(int) + relativeFileLocation.length() + SIGNITURE_SIZE){
    this->fileID = ID::clean(id);
    this->serverID = serverID;
    this->fileSize = fileSize;
    this->relativeFileLocation = relativePath;
    memcpy(this->signature, signature, SIGNITURE_SIZE);

    if (type == DataTypes::FILE_INDICATOR) {
        serialize();
    }
}

void FileIndicator::serialize() {
    unsigned char uuid[UUID_BYTE_SIZE];

    // ID
    ID::BytesFromString(this->fileID, uuid);
    memcpy(data+offset, uuid, UUID_BYTE_SIZE);
    offset += UUID_BYTE_SIZE;

    // Serverid
    ID::BytesFromString(this->serverID, uuid);
    memcpy(data+offset, uuid, UUID_BYTE_SIZE);
    offset += UUID_BYTE_SIZE;

    // File Hash
    memcpy(data+offset, this->signature, SIGNITURE_SIZE);
    offset += SIGNITURE_SIZE;

    // FileSize
    memcpy(data+offset, &this->fileSize, sizeof(this->fileSize));
    offset += sizeof(this->fileSize);

    // PathLength
    int len = this->relativeFileLocation.length();
    memcpy(data+offset, &len, sizeof(len));
    offset += sizeof(len);

    // Path String
    memcpy(data+offset, this->relativeFileLocation.data(), sizeof(len));
    offset += len;
}

FileIndicator FileIndicator::deserialize(unsigned char* serializedData) { 
    int offset = 4;

    // ID
    std::string fileID = ID::stringFromBytes(serializedData+offset);
    offset += UUID_BYTE_SIZE;

    // ServerID
    std::string serverID = ID::stringFromBytes(serializedData+offset);
    offset += UUID_BYTE_SIZE;

    // Signiture
    unsigned char signiture[SIGNITURE_SIZE];
    memcpy(signiture, serializedData+offset, SIGNITURE_SIZE);
    offset += SIGNITURE_SIZE;

    // File Size
    int filelen;
    memcpy(&filelen, serializedData+offset, sizeof(filelen));
    offset += sizeof(filelen);

    // path len
    int len;
    memcpy(&len, serializedData+offset, sizeof(len));
    offset += sizeof(len);

    // Path string
    std::string path(reinterpret_cast<char const*>(serializedData+offset), len);

    return FileIndicator(path, filelen, serverID, signiture, fileID, path, DataTypes::EMPTY);
}


void FileIndicator::onAddRequest(std::string serverID, std::string fileID, RemoteUser *requestee) {
    PrimaryClient* client = PrimaryClient::getInstance();

    Server* server = client->getServer(serverID);

    try {
        FileIndicator* fileIndicator = DatabaseConnection::getFileIndicatorFromDB(fileID);
        // Check requesting users perms
        if (fileIndicator->serverID != serverID) {
            return;
        }
        
        if (!server->knownUsers.contains(*requestee->getID())) { // If they dont belong to this server dont send them anything
            return;
        }

        requestee->connection.sendEncrypted(fileIndicator->getData(), fileIndicator->getDataLen());

        delete fileIndicator;


    } catch (std::runtime_error e) {
        return;
    }
}
void FileIndicator::onRemoveRequest(std::string serverID, std::string fileID, RemoteUser *requestee) {

}