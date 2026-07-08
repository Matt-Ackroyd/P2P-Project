#include "DataTypes.h"


JoinRequest::JoinRequest(DataTypes datatype, std::string givenInvitation) : Container(datatype, UUID_BYTE_SIZE) {
    this->invitation = givenInvitation;

    if (datatype != DataTypes::EMPTY) {
        serialize();
    }
}

void JoinRequest::serialize() {
    // inviation
    ID::BytesFromString(this->invitation, data+offset);
    offset += UUID_BYTE_SIZE;
}

// Returns the inviation as a string
std::string JoinRequest::deserialize(unsigned char* serializedData) {
    // The dataType has been removed by now
    int offset = sizeof(DataTypes);

    // ServerID
    std::string invitation = ID::stringFromBytes(serializedData+offset);

    return invitation;
}