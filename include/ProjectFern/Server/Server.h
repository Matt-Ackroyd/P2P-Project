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
    std::unordered_map<std::string, RemoteUser*> knownUsers;
    

    // All Active invitations to this server 
    std::unordered_set<std::string> activeInvitations;

    // Server Settings 

    // Server Roles

    // My Role?

public: 
    Server(std::string id = "");
    ~Server();
    std::string* getID();
    int addUser(RemoteUser *user, std::string invitation);
    int removeUser(RemoteUser *user);
    
    std::string createNewInvitation();

    void createNewTextChannel(std::string id = "");
    
    void loadChannel(TextChannel* channel);

    // All known TextChannels within this server 
    std::unordered_map<std::string, TextChannel*> knownChannels;
    std::unordered_set<std::string> onlineUsers;
    
};