// Other Users within a server
#pragma once
#include <vector>
#include <string>
#include "UDPConnection.h"

using namespace std;


class RemoteUser {
    string Username;
    int id;

    vector<string> knownEndpoints;
public:
    UDPConnection connection;
};