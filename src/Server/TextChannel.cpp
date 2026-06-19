#include "TextChannel.h"

TextChannel::TextChannel() {
    this->id.GenerateNewID();
}
std::string TextChannel::getID() {
    return this->id.get();
}