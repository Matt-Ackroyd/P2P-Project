#pragma once
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <filesystem>

#include "RemoteUser.h"
#include "TextChannel.h"


#define SERVER_PATH "/data/servers/"

class Server {
    std::string id;

    // Server Settings 

    // Server Roles

    // My Role?

public: 
    Server(std::string id = "");
    ~Server();
    std::string* getID();
    void addNewUser(RemoteUser *user);
    void loadUser(RemoteUser *user);
    int removeUser(RemoteUser *user);

    std::string createNewInvitation();

    void createNewTextChannel(std::string id = "");
    
    void loadChannel(TextChannel* channel);

    // All known TextChannels within this server 
    std::unordered_map<std::string, TextChannel*> knownChannels;
    std::unordered_map<std::string, RemoteUser*> knownUsers;
    std::unordered_set<std::string> onlineUsers;
    
};