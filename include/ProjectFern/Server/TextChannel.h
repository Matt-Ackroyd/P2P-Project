#pragma once
#include <unordered_map>
#include "DataTypes.h"

class TextChannel {
    ID id;

public:
    TextChannel();
    std::string getID();
};