#include "TextChannel.h"
#include "Server.h"

TextChannel::TextChannel(Server* server, std::string idString) {
    this->id = ID::clean(idString);
    this->ownedByThisServer = server;
}
std::string* TextChannel::getID() {
    return &this->id;
}

void TextChannel::loadMessages() {
    this->messages.clear();


}

Server *TextChannel::getServer() {
    return this->ownedByThisServer;
}
