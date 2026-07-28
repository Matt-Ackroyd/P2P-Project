#include "DataTypes.h"
#include "TextChannel.h"
#include "RemoteUser.h"



// Public Constructor
Request::Request(std::string serverID, std::string requestedID, DataTypes requestedDatatype) : Container(DataTypes::ADD_OR_MODIFY_REQUEST, sizeof(DataTypes) + UUID_BYTE_SIZE*2) {
    this->serverID = serverID;
    this->requestedID = requestedID;
    this->requestedDatatype = requestedDatatype;
    serialize();
}

// Private Constructor that doesn't need to be serilized
Request::Request(DataTypes dataTypeOfThisObject, std::string serverID, std::string requestedID, DataTypes requestedDatatype) : Container(dataTypeOfThisObject, 0) {
    this->serverID = serverID;
    this->requestedID = requestedID;
    this->requestedDatatype = requestedDatatype;
}


void Request::serialize() {
    unsigned char uuid[UUID_BYTE_SIZE];    

    // Serverid
    ID::BytesFromString(this->serverID, uuid);
    memcpy(data+offset, uuid, UUID_BYTE_SIZE);
    offset += UUID_BYTE_SIZE;

    // id
    ID::BytesFromString(this->requestedID, uuid);
    memcpy(data+offset, uuid, UUID_BYTE_SIZE);
    offset += UUID_BYTE_SIZE;

    // type
    memcpy(data+offset, &this->requestedDatatype, sizeof(DataTypes));
    offset += sizeof(DataTypes);
}

Request Request::deserialize(unsigned char* serializedData) {
    int offset = sizeof(DataTypes);

    std::string serverid = ID::stringFromBytes(serializedData+offset);
    offset += UUID_BYTE_SIZE;

    std::string id = ID::stringFromBytes(serializedData+offset);
    offset += UUID_BYTE_SIZE;

    DataTypes objectType;
    memcpy(&objectType, serializedData+offset, sizeof(objectType));
    offset += sizeof(objectType);

    return Request(serverid, id, objectType);
}

std::string Request::getRequestedID() {
    return requestedID;
}

DataTypes Request::getRequestedDatatype() {
    return requestedDatatype;
}

std::string Request::getServerID() {
    return this->serverID;
}

void Request::onRequest(unsigned char* decryptedData, RemoteUser* requestee) {
    Request request = Request::deserialize(decryptedData);

    request.getRequestedDatatype();

    switch(request.getRequestedDatatype()) {
        case DataTypes::TEXT_CHANNEL:
            TextChannel::onRequest(request.getServerID(), request.getRequestedID(), requestee);
            break;
        case DataTypes::VOICE_CHANNEL:
            break;
        case DataTypes::MESSAGETYPE: 
            MessageContainer::onRequest(request.getServerID(), request.getRequestedID(), requestee);
            break;
        case DataTypes::FILE_INDICATOR:
            FileIndicator::onRequest(request.getServerID(), request.getRequestedID(), requestee);
            break;
        case DataTypes::USER:
            RemoteUser::onRequest(request.getServerID(), request.getRequestedID(), requestee);
            break;
        case DataTypes::DELETED:
            break;
    }
}