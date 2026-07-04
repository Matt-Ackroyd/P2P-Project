#include "RemoteUser.h"

RemoteUser::RemoteUser(ID* id) {
    this->id = ID::fromBytes(id->getRaw());
}

RemoteUser::~RemoteUser() {
}

ID* RemoteUser::getID() {
    return &this->id;
}