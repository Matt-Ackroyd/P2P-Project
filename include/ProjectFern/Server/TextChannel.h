#pragma once
#include <vector>
#include "DataTypes.h"

class Server;

class TextChannel {
    ID id;
    Server* ownedByThisServer;

public:
    // Contains the last 100 or so messages by default
    std::vector<MessageContainer*> messages;
    TextChannel(Server* ownedByThisServer, std::string idString = "NULL");
    ID* getID();

    void loadMessages();
};