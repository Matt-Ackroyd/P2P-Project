#pragma once
#include <vector>
#include "DataTypes.h"

class TextChannel {
    ID id;

public:
    std::vector<MessageContainer*> messages;
    TextChannel();
    ID* getID();
};