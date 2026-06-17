#pragma once
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include "RemoteUser.h"
#include "TextChannel.h"



class Server {
    ID id;
    std::unordered_map<std::string, RemoteUser*> knownUsers;
    std::unordered_set<std::string> onlineUsers;

    // All Active invitations to this server 
    std::unordered_set<std::string> activeInvitations;

    // All known TextChannels within this server 
    std::unordered_map<int, TextChannel> knownChannels;

    // Server Settings 

    // Server Roles

    // My Role?

public: 
    ~Server();
    int addUser(RemoteUser *user, std::string invitation);
    int removeUser(RemoteUser *user);
    
    std::string createNewInvitation();
};