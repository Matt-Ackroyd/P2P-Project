// Other Users within a server
#include <vector>
#include <string>
#include "../UDPConection/UDPConnection.h"

using namespace std;


class User {
    string Username;
    int id;


    vector<string> knownEndpoints;
    UDPConnection connection;

    char* sharedSecret;
};