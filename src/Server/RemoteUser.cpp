#include "RemoteUser.h"

RemoteUser::RemoteUser(std::string id) {
    this->id = id;
}

RemoteUser::~RemoteUser() {
}

std::string* RemoteUser::getID() {
    return &this->id;
}