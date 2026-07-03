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
    ID id;
    std::unordered_map<std::string, RemoteUser*> knownUsers;
    std::unordered_set<std::string> onlineUsers;

    // All Active invitations to this server 
    std::unordered_set<std::string> activeInvitations;

    // Server Settings 

    // Server Roles

    // My Role?

public: 
    Server(std::string id = "NULL");
    ~Server();
    ID* getID();
    int addUser(RemoteUser *user, std::string invitation);
    int removeUser(RemoteUser *user);
    
    std::string createNewInvitation();

    int createNewChannel();
    //loads a channel from file
    void loadChannel(std::string id);
    //loads ALL channel from file
    void loadAllChannels();

    // All known TextChannels within this server 
    std::unordered_map<std::string, TextChannel*> knownChannels;
};