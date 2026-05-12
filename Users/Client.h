// TODO: Needs Better name
// Acts as class that stores information about this particular client, where user refers to other clients 
#include "Server.h"
#include <vector>

using namespace std;

class Client {
    vector<Server> allServers;
    string defaultUsername;
};