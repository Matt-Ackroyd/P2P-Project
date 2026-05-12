#include "Server.h"

int Server::addUser(User user) {
    this->knownUsers.push_back(user);
    return 1;
}
