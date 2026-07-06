#include "DataTypes.h"
#include "Server.h"

// Public Constructor
ServerContainer::ServerContainer(DataTypes datatype, Server* server) : Container(datatype, UUID_BYTE_SIZE) {
    this->serverID = *server->getID();
    serialize();
}

// Private Constructor
ServerContainer::ServerContainer(std::string id) : Container(DataTypes::EMPTY, 0) {
    this->serverID = id;
}

void ServerContainer::serialize() {
    unsigned char uuid[UUID_BYTE_SIZE];

    // ServerID
    ID::BytesFromString(this->serverID, uuid);
    memcpy(data+offset, uuid, UUID_BYTE_SIZE);
    offset += UUID_BYTE_SIZE;
}

ServerContainer ServerContainer::deserialize(unsigned char* serializedData) {
    // The dataType has been removed by now
    int offset = sizeof(DataTypes);

    // ServerID
    std::string serverID = ID::stringFromBytes(serializedData+offset);
    offset += UUID_BYTE_SIZE;

    return ServerContainer(serverID);
}

std::string ServerContainer::getServerID() {
    return this->serverID;
}