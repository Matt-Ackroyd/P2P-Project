// Other Users within a server
#pragma once
#include <vector>
#include <string>
#include "UDPConnection.h"

using namespace std;


class RemoteUser {
    string Username;
    UUID *id;

    vector<string> knownEndpoints;
public:
    RemoteUser(UUID id, unsigned char* sharedSecret);
    ~RemoteUser();
    UDPConnection* connection;
};