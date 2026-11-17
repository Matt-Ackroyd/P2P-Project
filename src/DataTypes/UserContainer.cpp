#include "DataTypes.h"
#include "Server.h"
#include "RemoteUser.h"
#include "PrimaryClient.h"

// Public contructor
UserContainer::UserContainer(DataTypes datatype, RemoteUser* user, Server* server) 
: Container(datatype, USER_CONTAINER_SIZE) {
    this->userID = *user->getID();
    this->serverID = *server->getID();
    this->contactAddress = user->contactAddress;
    this->contactPort = user->contactPort;
    this->requiresRelay = user->requiresRelay;
    serialize();
}

// Private Constructor
UserContainer::UserContainer(std::string userID, std::string serverID, int contactAddress, short int contactPort, bool requiresRelay) : Container(DataTypes::EMPTY, 0) {
    this->userID = userID;
    this->serverID = serverID;
    this->contactAddress = contactAddress;
    this->contactPort = contactPort;
    this->requiresRelay = requiresRelay;
}

void UserContainer::serialize() {
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

UserContainer UserContainer::deserialize(unsigned char* serializedData) {
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

    return UserContainer(userID, serverID, contactAddress, contactPort, requiresRelay);
}

std::string UserContainer::getServerID() {
    return this->serverID;
}
std::string UserContainer::getUserID() {
    return this->userID;
}

int UserContainer::getContactAddress() {
    return contactAddress;
}
short int UserContainer::getContactPort() {
    return contactPort;
}
bool UserContainer::getRelayRequired() {
    return this->requiresRelay;
}




void UserContainer::onReceived(unsigned char* decryptedData) {
    // Add check that the user creating this channel has the permision to do so
    UserContainer request = UserContainer::deserialize(decryptedData);

    PrimaryClient* client = PrimaryClient::getInstance();

    client->registerNewUser(request.getUserID());
    RemoteUser* newUser = client->getUser(request.getUserID());

    newUser->contactAddress = request.getContactAddress();
    newUser->contactPort = request.getContactPort();

    client->getServer(request.getServerID())->addNewUser(newUser);
}

void UserContainer::onReceived(UserContainer request) {
    // Add check that the user creating this channel has the permision to do so

    PrimaryClient* client = PrimaryClient::getInstance();

    client->registerNewUser(request.getUserID());
    RemoteUser* newUser = client->getUser(request.getUserID());

    newUser->contactAddress = request.getContactAddress();
    newUser->contactPort = request.getContactPort();

    client->getServer(request.getServerID())->addNewUser(newUser);
}