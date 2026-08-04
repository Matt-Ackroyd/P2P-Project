#include "DataTypes.h"
#include "Server.h"
#include "PrimaryClient.h"

// Public Constructor
ServerContainer::ServerContainer(DataTypes datatype, Server* server, RemoteUser* user) : serverUser(DataTypes::USER, user, server), Container(datatype, UUID_BYTE_SIZE*2 + UserContainer::USER_CONTAINER_SIZE) {
    this->serverID = *server->getID();    

    serialize();
}

// Private Constructor
ServerContainer::ServerContainer(std::string id, UserContainer user) : serverUser(user), Container(DataTypes::EMPTY, 0) {
    this->serverID = id;
}

void ServerContainer::serialize() {
    unsigned char uuid[UUID_BYTE_SIZE];

    // ServerID
    ID::BytesFromString(this->serverID, uuid);
    memcpy(data+offset, uuid, UUID_BYTE_SIZE);
    offset += UUID_BYTE_SIZE;

    // Connected User info (offset by length of datatypes because we already know this is a)
    memcpy(data+offset, this->serverUser.getData(), this->serverUser.getDataLen());
    offset += this->serverUser.getDataLen();
}

ServerContainer ServerContainer::deserialize(unsigned char* serializedData) {
    // The dataType has been removed by now
    int offset = sizeof(DataTypes);

    // ServerID
    std::string serverID = ID::stringFromBytes(serializedData+offset);
    offset += UUID_BYTE_SIZE;

    UserContainer user = UserContainer::deserialize(serializedData+offset);

    return ServerContainer(serverID, user);
}

std::string ServerContainer::getServerID() {
    return this->serverID;
}

UserContainer ServerContainer::getServerUser() {
    return this->serverUser;
}



void ServerContainer::onReceived(unsigned char* decryptedData) {
    // Add some sort of check to ensure that we asked to join this server before making it
    PrimaryClient* client = PrimaryClient::getInstance();

    ServerContainer serverContainer = ServerContainer::deserialize(decryptedData);

    client->createNewServer(serverContainer.getServerID());

    UserContainer::onReceived(serverContainer.getServerUser());

    RemoteUser* serverUser = client->getUser(serverContainer.getServerUser().getUserID());
    Server* server = client->getServer(serverContainer.getServerID());
    
    serverUser->connection.sendSyncRequest(server);
}

