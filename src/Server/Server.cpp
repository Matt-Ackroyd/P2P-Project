#include "Server.h"

Server::Server() {
    //TEMP
    this->id.GenerateNewID();
}
Server::~Server() {
    for (auto user : knownUsers) {
        delete user.second;
    }
}

std::string Server::getID() {
    return id.get();
}

int Server::addUser(RemoteUser *user, std::string invitation) {
    // Client Side Check for perms TODO

    // Make sure the request has a valid invitation
    if (this->activeInvitations.count(invitation)) {
        this->knownUsers[user->getID()] = user;
    }

    // Broadcast to the network about the new addition TODO

    return 1;
}


std::string Server::createNewInvitation() {
    // TODO change to a more in depth method
    ID invitation;
    invitation.GenerateNewID();

    this->activeInvitations.insert(invitation.get());
    return invitation.get();
}