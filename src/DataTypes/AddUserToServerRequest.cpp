#include "DataTypes.h"
#include "Server.h"
#include "RemoteUser.h"

// Public contructor
AddUserToServerRequest::AddUserToServerRequest(DataTypes datatype, RemoteUser* user, Server* server) 
: Container(datatype, (UUID_BYTE_SIZE*2 + sizeof(int) + sizeof(short int) + sizeof(bool))) {
    this->userID = *user->getID();
    this->serverID = *server->getID();
    this->contactAddress = user->contactAddress;
    this->contactPort = user->contactPort;
    this->requiresRelay = user->requiresRelay;
    serialize();
}

// Private Constructor
AddUserToServerRequest::AddUserToServerRequest(std::string userID, std::string serverID, int contactAddress, short int contactPort, bool requiresRelay) : Container(DataTypes::EMPTY, 0) {
    this->userID = userID;
    this->serverID = serverID;
    this->contactAddress = contactAddress;
    this->contactPort = contactPort;
    this->requiresRelay = requiresRelay;
}

void AddUserToServerRequest::serialize() {
    unsigned char uuid[UUID_BYTE_SIZE];

    // UserID
    ID::BytesFromString(this->userID, uuid);
    memcpy(data+offset, uuid, UUID_BYTE_SIZE);
    offset += UUID_BYTE_SIZE;

    // serverID
    ID::BytesFromString(this->serverID, uuid);
    memcpy(data+offset, uuid, UUID_BYTE_SIZE);
    offset += UUID_BYTE_SIZE;

    // Contact Adress
    memcpy(data+offset, &this->contactAddress, sizeof(this->contactAddress));
    offset += sizeof(this->contactAddress);

    // Contact Port
    memcpy(data+offset, &this->contactPort, sizeof(this->contactPort));
    offset += sizeof(this->contactPort);

    // Requires Relay?
    memcpy(data+offset, &this->requiresRelay, sizeof(this->requiresRelay));
    offset += sizeof(this->requiresRelay);
}

AddUserToServerRequest AddUserToServerRequest::deserialize(unsigned char* serializedData) {
    // The dataType has been removed by now
    int offset = sizeof(DataTypes);

    // UserID
    std::string userID = ID::stringFromBytes(serializedData+offset);
    offset += UUID_BYTE_SIZE;

    // ServerID
    std::string serverID = ID::stringFromBytes(serializedData+offset);
    offset += UUID_BYTE_SIZE;

    // Contact Adress
    int contactAddress;
    memcpy(&contactAddress, serializedData+offset, sizeof(contactAddress));
    offset += sizeof(contactAddress);

    // Contact Port
    short int contactPort;
    memcpy(&contactPort, serializedData+offset, sizeof(contactPort));
    offset += sizeof(contactPort);

    // Requires Relay?
    bool requiresRelay;
    memcpy(&requiresRelay, serializedData+offset, sizeof(requiresRelay));
    offset += sizeof(requiresRelay);

    return AddUserToServerRequest(userID, serverID, contactAddress, contactPort, requiresRelay);
}

std::string AddUserToServerRequest::getServerID() {
    return this->serverID;
}
std::string AddUserToServerRequest::getUserID() {
    return this->userID;
}

int AddUserToServerRequest::getContactAddress() {
    return contactAddress;
}
short int AddUserToServerRequest::getContactPort() {
    return contactPort;
}
bool AddUserToServerRequest::getRelayRequired() {
    return this->requiresRelay;
}