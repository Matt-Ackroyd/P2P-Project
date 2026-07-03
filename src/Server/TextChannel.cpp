#include "TextChannel.h"
#include "Server.h"

TextChannel::TextChannel(Server* server, std::string idString) {
    this->id = ID(idString);
    this->ownedByThisServer = server;
}
ID* TextChannel::getID() {
    return &this->id;
}

void TextChannel::loadMessages() {
    this->messages.clear();


}