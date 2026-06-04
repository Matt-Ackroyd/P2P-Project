#pragma once
#include "RemoteUser.h"
#include <vector>

using namespace std;


class Server {
    vector<RemoteUser> knownUsers;
    vector<RemoteUser> onlineUsers;
    vector<string> activeInvitations;

    public: 
        int addUser(RemoteUser user);
        int removeUser(RemoteUser user);
};