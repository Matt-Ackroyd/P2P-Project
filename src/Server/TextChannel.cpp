#include "TextChannel.h"

TextChannel::TextChannel() {
    this->id.GenerateNewID();
}
ID* TextChannel::getID() {
    return &this->id;
}