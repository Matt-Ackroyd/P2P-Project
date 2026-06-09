// Other Users within a server
#pragma once
#include <string>
#include "UDPConnection.h"

using namespace std;


class RemoteUser {
    string Username;
    UUID *id;

public:
    RemoteUser(UUID id, unsigned char* sharedSecret);
    ~RemoteUser();
    string getID();
    UDPConnection* connection;
};