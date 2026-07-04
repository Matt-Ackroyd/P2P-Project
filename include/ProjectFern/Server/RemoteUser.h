// Other Users within a server
#pragma once
#include <string>
#include "UDPConnection.h"


class RemoteUser {  
    ID id;
    
public:
    RemoteUser(ID* id);
    ~RemoteUser();
    ID* getID();
    UDPConnection connection;

    std::string Username;
    boolean requiresRelay;
    std::string contactAdress;
    int contactPort;
};