#pragma once
#include <vector>
#include <unordered_map>
#include "RemoteUser.h"

using namespace std;


class Server {
    UUID id;
    unordered_map<string, RemoteUser> knownUsers;
    vector<RemoteUser> onlineUsers;
    vector<string> activeInvitations;

    public: 
        int addUser(RemoteUser user);
        int removeUser(RemoteUser user);
};