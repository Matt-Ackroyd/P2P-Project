// Other Users within a server
#pragma once
#include <string>
#include "UDPConnection.h"


class RemoteUser {
    std::string Username;
    ID *id;

public:
    RemoteUser(ID id, unsigned char* sharedSecret);
    ~RemoteUser();
    std::string getID();
    UDPConnection* connection;
};