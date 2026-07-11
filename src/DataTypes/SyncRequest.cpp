#include "Sync.h"
#include "PrimaryClient.h"

// Private Constructor Used in the deserialize function, doesn't need to alocate memory for serialization
SyncRequest::SyncRequest(std::string syncID, std::string serverID, DataTypes objectType, int rangeStart, int rangeEnd) : Container(DataTypes::EMPTY, 0) {
    this->syncID = syncID;
    this->objectType = objectType;
    this->serverID = serverID;
    this->rangeStart = rangeStart;
    this->rangeEnd = rangeEnd;
}

SyncRequest::SyncRequest(std::string serverID, DataTypes objectType, int rangeStart, int rangeEnd) : 
Container(DataTypes::SYNC, sizeof(DataTypes)+UUID_BYTE_SIZE+sizeof(int)*2) {
    this->syncID = ID::GenerateNewID();
    this->serverID = serverID;
    this->objectType = objectType;
    this->rangeStart = rangeStart;
    this->rangeEnd = rangeEnd;
    serialize();
}

void SyncRequest::serialize() {
    unsigned char uuid[UUID_BYTE_SIZE];    

    // syncID
    ID::BytesFromString(this->syncID, uuid);
    memcpy(data+offset, uuid, UUID_BYTE_SIZE);
    offset += UUID_BYTE_SIZE;

    // server
    ID::BytesFromString(this->serverID, uuid);
    memcpy(data+offset, uuid, UUID_BYTE_SIZE);
    offset += UUID_BYTE_SIZE;

    // objectType
    memcpy(data+offset, &this->objectType, sizeof(this->objectType));
    offset += sizeof(this->objectType);

    // rangeStart
    memcpy(data+offset, &this->rangeStart, sizeof(this->rangeStart));
    offset += sizeof(this->rangeStart);

    // rangeEnd
    memcpy(data+offset, &this->rangeEnd, sizeof(this->rangeEnd));
    offset += sizeof(this->rangeEnd);
}

SyncRequest SyncRequest::deserialize(unsigned char* serializedData) {
    // syncID
    int offset = sizeof(DataTypes);

    std::string syncID = ID::stringFromBytes(serializedData+offset);
    offset += UUID_BYTE_SIZE;

    // server
    std::string serverID = ID::stringFromBytes(serializedData+offset);
    offset += UUID_BYTE_SIZE;

    // objectType
    DataTypes objectType;
    memcpy(&objectType, serializedData+offset, sizeof(objectType));
    offset += sizeof(objectType);

    // rangeStart
    int rangeStart;
    memcpy(&rangeStart, serializedData+offset, sizeof(rangeStart));
    offset += sizeof(rangeStart);

    // rangeEnd
    int rangeEnd;
    memcpy(&rangeEnd, serializedData+offset, sizeof(rangeEnd));
    offset += sizeof(rangeEnd);

    return SyncRequest(syncID, serverID, objectType, rangeStart, rangeEnd);
}

std::string SyncRequest::getSyncID() {
    return syncID;
}
DataTypes SyncRequest::getObjectType() {
    return objectType;
}
std::string SyncRequest::getserverID() {
    return serverID;
}
int SyncRequest::getStartRange() {
    return rangeStart;
}
int SyncRequest::getEndRange() {
    return rangeEnd;
}



void onSyncRequest(unsigned char* decryptedData, RemoteUser* requestee) {
    PrimaryClient* client = PrimaryClient::getInstance();
    SyncRequest request = SyncRequest::deserialize(decryptedData);

    Server* server = client->getServer(request.getserverID());
    // If the user requesting this sync does not belong to the server DO NOT send any info
    if (!server->knownUsers.contains(*requestee->getID())) {
        return;
    }

    SyncResponse response = SyncResponse::generateResponse(&request);
    requestee->connection.sendEncrypted(response.getData(), response.getDataLen());
}