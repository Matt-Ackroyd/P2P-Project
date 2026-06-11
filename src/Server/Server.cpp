#include "Server.h"

Server::~Server() {
    for (auto user : knownUsers) {
        delete user.second;
    }
}

int Server::addUser(RemoteUser *user, string invitation) {
    // Client Side Check for perms TODO

    // Make sure the request has a valid invitation
    if (this->activeInvitations.count(invitation)) {
        this->knownUsers[user->getID()] = user;
    }

    // Broadcast to the network about the new addition TODO

    return 1;
}


string Server::createNewInvitation() {
    // TODO change to a more in depth method
    ID invitation;
    invitation.GenerateNewID();

    this->activeInvitations.insert(invitation.get());
    return invitation.get();
}