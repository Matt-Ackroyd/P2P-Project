#include "RemoteUser.h"

RemoteUser::RemoteUser(ID* id) {
    this->id = ID::fromBytes(id->getRaw());
}

RemoteUser::~RemoteUser() {
}

std::string RemoteUser::getID() {
    return this->id.getString();
}