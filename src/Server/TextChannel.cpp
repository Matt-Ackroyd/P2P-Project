#include "TextChannel.h"
#include "Server.h"
#include "PrimaryClient.h"
#include "DatabaseConnection.h"

TextChannel::TextChannel(Server* server, std::string idString) {
    this->id = ID::clean(idString);
    this->ownedByThisServer = server;
    DatabaseConnection::getMessagesFromDB(this, 100);
}
std::string* TextChannel::getID() {
    return &this->id;
}

void TextChannel::loadMessage(MessageContainer* message) {
    this->messages.push_back(message);
}

void TextChannel::sendMessage(MessageContainer* message) {
    this->messages.push_back(message);
    DatabaseConnection::addMessageToDB(this, message);

    // REPLACE WITH CHANNEL SPECIFIC RECIPIENTS CHECK ROLE PERMS WHEN THATS ADDED AND CHANGE TO ONLINE USERS 
    for (auto& [recipientID, recipient]: this->getServer()->knownUsers) {
        recipient->connection.sendEncrypted(message->getData(), message->getDataLen());
    }
}

Server *TextChannel::getServer() {
    return this->ownedByThisServer;
}
