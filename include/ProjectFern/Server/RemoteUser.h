// Other Users within a server
#pragma once
#include <string>
#include "UDPConnection.h"


class RemoteUser {  
    std::string id;
    
public:
    RemoteUser(std::string id);
    ~RemoteUser();
    std::string* getID();
    UDPConnection connection;

    std::string Username = "Unknown";
    bool requiresRelay;
    int contactAddress;
    short int contactPort;
};