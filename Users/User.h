// Other Users within a server
#include <vector>
#include <string>
#include "../UDPConnection/UDPConnection.h"

using namespace std;


class User {
    string Username;
    int id;


    vector<string> knownEndpoints;
    UDPConnection connection;

    char* sharedSecret;
};