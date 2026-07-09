#include "DataTypes.h"


JoinRequest::JoinRequest(DataTypes datatype, std::string givenInvitation, int contactAddress, short int contactPort, bool requiresRelay) 
: Container(datatype, UUID_BYTE_SIZE + sizeof(int) + sizeof(short int) + sizeof(bool)) {
    this->invitation = givenInvitation;
    this->contactAddress = contactAddress;
    this->contactPort = contactPort;
    this->requiresRelay = requiresRelay;

    serialize();    
}

JoinRequest::JoinRequest(std::string givenInvitation, int contactAddress, short int contactPort, bool requiresRelay) : Container(DataTypes::EMPTY, 0) {
    this->invitation = givenInvitation;
    this->contactAddress = contactAddress;
    this->contactPort = contactPort;
    this->requiresRelay = requiresRelay;
}

void JoinRequest::serialize() {
    // inviation
    ID::BytesFromString(this->invitation, data+offset);
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

// Returns the inviation as a string
JoinRequest JoinRequest::deserialize(unsigned char* serializedData) {
    // The dataType has been removed by now
    int offset = sizeof(DataTypes);

    // ServerID
    std::string invitation = ID::stringFromBytes(serializedData+offset);
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

    return JoinRequest(invitation, contactAddress, contactPort, requiresRelay);
}

std::string JoinRequest::getInvitation()
{
    return this->invitation;
}

int AddUserToServerRequest::getContactAddress() {
    return contactAddress;
}
short int AddUserToServerRequest::getContactPort() {
    return contactPort;
}
