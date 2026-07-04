#include "Server.h"

Server::Server(std::string id) {
    this->id = ID(id);
}
Server::~Server() {
    for (auto user : knownUsers) {
        delete user.second;
    }
}

ID* Server::getID() {
    return &id;
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

    this->activeInvitations.insert(invitation.getString());
    return invitation.getString();
}

void Server::loadAllChannels() {
    std::filesystem::path path(SERVER_PATH + this->id.getString() + "/TextChannels/");

    for (const auto & entry : std::filesystem::directory_iterator(path)) {
        loadChannel(entry.path().filename().string());
    }
}

void Server::loadChannel(std::string idString) {
    std::filesystem::path path(SERVER_PATH + this->id.getString() + "/TextChannels/" + idString);

    TextChannel* channel = new TextChannel(this, idString);

}