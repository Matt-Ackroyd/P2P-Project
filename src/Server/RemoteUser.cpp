#include "RemoteUser.h"

RemoteUser::RemoteUser(UUID id, unsigned char* sharedSecret) {
    this->id = new UUID;
    this->id->set(id.get_raw());
    this->connection = new UDPConnection(sharedSecret);
}

RemoteUser::~RemoteUser() {
    delete this->connection;
}