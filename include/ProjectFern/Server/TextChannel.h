#pragma once
#include <vector>
#include "DataTypes.h"

class Server;

class TextChannel {
    std::string id;
    Server* ownedByThisServer;

public:
    // Contains the last 100 or so messages by default
    std::vector<MessageContainer*> messages;
    TextChannel(Server* ownedByThisServer, std::string idString = "");
    std::string* getID();

    void loadMessages();
    
    Server* getServer();
};