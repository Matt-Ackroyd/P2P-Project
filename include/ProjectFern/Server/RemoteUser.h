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

    std::string Username;
    bool requiresRelay;
    int contactAdress;
    short int contactPort;
};