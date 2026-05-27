// Other Users within a server
#include <vector>
#include <string>
#include "UDPConnection.h"

using namespace std;


class RemoteUser {
    string Username;
    int id;

    vector<string> knownEndpoints;
    UDPConnection connection;

    unsigned char* sharedSecret;
};