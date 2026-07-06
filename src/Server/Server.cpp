#include "Server.h"
#include "DatabaseConnection.h"

Server::Server(std::string id) {
    this->id = ID::clean(id);
    DatabaseConnection::getTextChannelsFromDB(this);
    DatabaseConnection::getUsersInServerFromDB(this);
}

Server::~Server() {
    for (auto user : knownUsers) {
        delete user.second;
    }
}

std::string* Server::getID() {
    return &id;
}

void Server::addNewUser(RemoteUser *user) {
    // If the User is already in this server abort
    if (this->knownUsers.contains(*user->getID())) {
        return;
    }

    DatabaseConnection::addUserToServerDB(user, this);
    loadUser(user);
}

void Server::loadUser(RemoteUser *user) {
    if (user == nullptr) {
        return;
    }

    this->knownUsers[*user->getID()] = user;
}

std::string Server::createNewInvitation() {
    // TODO change to a more in depth method
    std::string invitation = ID::GenerateNewID();

    DatabaseConnection::addServerInvitationToDB(invitation, this);
    return invitation;
}

void Server::createNewTextChannel(std::string id) {
    TextChannel* channel = new TextChannel(this, ID::clean(id));
    DatabaseConnection::addTextChannelToDB(this, channel);

    loadChannel(channel);
    CppInterface::instancePtr->loadChannel(channel);
}

void Server::loadChannel(TextChannel* channel) {
    this->knownChannels[*channel->getID()] = channel;
}