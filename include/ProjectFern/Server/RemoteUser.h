// Other Users within a server
#pragma once
#include <string>
#include "UDPConnection.h"

using namespace std;


class RemoteUser {
    string Username;
    ID *id;

public:
    RemoteUser(ID id, unsigned char* sharedSecret);
    ~RemoteUser();
    string getID();
    UDPConnection* connection;
};