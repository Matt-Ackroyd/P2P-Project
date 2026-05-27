#include "Server.h"

int Server::addUser(RemoteUser user) {
    this->knownUsers.push_back(user);
    return 1;
}
