#include "DataTypes.h"
#include "PrimaryClient.h"
#include "DatabaseConnection.h"

MessageContainer::MessageContainer(DataTypes datatype, std::string server, std::string channel, std::string author, std::string message, std::string messageid) 
    : Container(datatype, (UUID_BYTE_SIZE*4 + sizeof(int) + message.length())) {
    this->serverID = server;
    this->channelID = channel;
    this->author = author;
    this->message = message;
    this->messageLength = message.length();
    this->messageID = ID::clean(messageid);

    if (datatype != DataTypes::EMPTY) {
        serialize();
    }
    
}


void MessageContainer::serialize() {
    unsigned char uuid[UUID_BYTE_SIZE];    

    // MessageID
    ID::BytesFromString(this->messageID, uuid);
    memcpy(data+offset, uuid, UUID_BYTE_SIZE);
    offset += UUID_BYTE_SIZE;

    // ServerID
    ID::BytesFromString(this->serverID, uuid);
    memcpy(data+offset, uuid, UUID_BYTE_SIZE);
    offset += UUID_BYTE_SIZE;

    // ChannelID
    ID::BytesFromString(this->channelID, uuid);
    memcpy(data+offset, uuid, UUID_BYTE_SIZE);
    offset += UUID_BYTE_SIZE;

    // Author
    ID::BytesFromString(this->author, uuid);
    memcpy(data+offset, uuid, UUID_BYTE_SIZE);
    offset += UUID_BYTE_SIZE;

    // Message Length
    memcpy(data+offset, &this->messageLength, sizeof(this->messageLength));
    offset += sizeof(this->messageLength);

    // Message
    memcpy(data+offset, this->message.data(), this->messageLength);
    offset += this->messageLength;
}

MessageContainer* MessageContainer::deserialize(unsigned char* serializedData) {
    // The dataType has been removed by now
    int offset = sizeof(DataTypes);

    // MessageID
    std::string messageID = ID::stringFromBytes(serializedData+offset);
    offset += UUID_BYTE_SIZE;

    // ServerID
    std::string serverID = ID::stringFromBytes(serializedData+offset);
    offset += UUID_BYTE_SIZE;

    // ChannelID
    std::string channelID = ID::stringFromBytes(serializedData+offset);
    offset += UUID_BYTE_SIZE;

    // Author
    std::string author = ID::stringFromBytes(serializedData+offset);
    offset += UUID_BYTE_SIZE;

    // Message Length
    int msglen = 0;
    memcpy(&msglen, serializedData+offset, sizeof(msglen));
    offset += sizeof(msglen);

    // Message
    std::string msg(reinterpret_cast<char const*>(serializedData+offset), msglen);
    offset += msglen;

    return new MessageContainer(DataTypes::EMPTY, serverID, channelID, author, msg, messageID);
}

std::string* MessageContainer::getMessageID() {
    return &this->messageID;
}

std::string* MessageContainer::getServerID() {
    return &this->serverID;
}
std::string* MessageContainer::getChannelID() {
    return &this->channelID;
}
std::string* MessageContainer::getAuthor() {
    return &this->author;
}
std::string MessageContainer::getMessage() {
    return this->message;
}

void MessageContainer::onRequest(std::string serverID, std::string id, RemoteUser *requestee) {
    PrimaryClient* client = PrimaryClient::getInstance();

    Server* server = client->getServer(serverID);
    TextChannel* channel = server->getChannel(id);

    try {
        MessageContainer message = DatabaseConnection::getMessageFromDB(id);
        // Check requesting users perms
        if (message.serverID != serverID) {
            return;
        }
        
        if (!server->knownUsers.contains(*requestee->getID())) { // If they dont belong to this server dont send them anything
            return;
        }

        requestee->connection.sendEncrypted(message.getData(), message.getDataLen());


    } catch (std::runtime_error e) {
        return;
    }

    

}
