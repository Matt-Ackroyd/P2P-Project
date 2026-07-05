#include "Server.h"

Server::Server(std::string id) {
    this->id = ID::clean(id);
}
Server::~Server() {
    for (auto user : knownUsers) {
        delete user.second;
    }
}

std::string* Server::getID() {
    return &id;
}

int Server::addUser(RemoteUser *user, std::string invitation) {
    // Client Side Check for perms TODO

    // Make sure the request has a valid invitation
    if (this->activeInvitations.count(invitation)) {
        this->knownUsers[*user->getID()] = user;
    }

    // Broadcast to the network about the new addition TODO

    return 1;
}


std::string Server::createNewInvitation() {
    // TODO change to a more in depth method
    std::string invitation = ID::GenerateNewID();

    this->activeInvitations.insert(invitation);
    return invitation;
}



void Server::loadChannel(TextChannel* channel) {
    this->knownChannels[*channel->getID()] = channel;
}