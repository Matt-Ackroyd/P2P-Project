// Other Users within a server
#pragma once
#include <string>
#include "UDPConnection.h"


class RemoteUser {
    std::string Username;
    ID id;

public:
    RemoteUser(ID* id);
    ~RemoteUser();
    std::string getID();
    UDPConnection connection;
};