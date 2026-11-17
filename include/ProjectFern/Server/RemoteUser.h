// Other Users within a server
#pragma once
#include <string>
#include "UDPConnection.h"


class RemoteUser {  
    std::string id;
    
public:
    RemoteUser(std::string id);
    ~RemoteUser();
    UDPConnection connection;
    

    std::string Username = "Unknown";
    bool requiresRelay;
    int contactAddress;
    u_short contactPort;

    std::string* getID();
    static void onRequest(std::string serverID, std::string id, RemoteUser *requestee);
};