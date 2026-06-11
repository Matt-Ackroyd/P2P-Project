#include "RemoteUser.h"

RemoteUser::RemoteUser(ID id, unsigned char* sharedSecret) {
    this->id = new ID;
    this->id->set(id.getRaw());
    this->connection = new UDPConnection(sharedSecret);
}

RemoteUser::~RemoteUser() {
    delete this->connection;
}

string RemoteUser::getID() {
    return this->id->get();
}