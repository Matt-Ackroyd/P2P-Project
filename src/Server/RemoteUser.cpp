#include "RemoteUser.h"
#include "PrimaryClient.h"

RemoteUser::RemoteUser(std::string id) {
    this->id = id;
}

RemoteUser::~RemoteUser() {
}

std::string* RemoteUser::getID() {
    return &this->id;
}


void RemoteUser::onRequest(std::string serverID, std::string id, RemoteUser *requestee) {
    PrimaryClient* client = PrimaryClient::getInstance();

    Server* server = client->getServer(serverID);
    RemoteUser* user = client->getUser(id);

    if (user == nullptr) {
        return;
    }

    // Check requesting users perms
    if (!server->knownUsers.contains(*requestee->getID())) { // If they dont belong to this server dont send them anything
        return;
    }

    UserContainer outgoing(DataTypes::USER, user, server);
    requestee->connection.sendEncrypted(outgoing.getData(), outgoing.getDataLen());

}