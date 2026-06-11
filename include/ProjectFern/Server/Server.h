#pragma once
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include "RemoteUser.h"
#include "TextChannel.h"


using namespace std;


class Server {
    ID id;
    unordered_map<string, RemoteUser*> knownUsers;
    unordered_set<string> onlineUsers;

    // All Active invitations to this server 
    unordered_set<string> activeInvitations;

    // All known TextChannels within this server 
    unordered_map<int, TextChannel> knownChannels;

    // Server Settings 

    // Server Roles

    // My Role?

public: 
    ~Server();
    int addUser(RemoteUser *user, string invitation);
    int removeUser(RemoteUser *user);
    
    string createNewInvitation();
};