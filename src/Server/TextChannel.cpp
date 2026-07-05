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

void TextChannel::sendMessage(MessageContainer* message, int len) {
    this->messages.push_back(message);
    DatabaseConnection::addMessageToDB(this, message);

    // REPLACE WITH CHANNEL SPECIFIC RECIPIENTS CHECK ROLE PERMS WHEN THATS ADDED
    for (auto& recipientID: this->getServer()->onlineUsers) {
        RemoteUser* recipient = PrimaryClient::getInstance()->getUser(recipientID);
        unsigned char data[len];
        message->serialize(data);
        recipient->connection.sendEncrypted(data, len);
    }
}

Server *TextChannel::getServer() {
    return this->ownedByThisServer;
}
