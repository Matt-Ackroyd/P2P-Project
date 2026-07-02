#include "DataTypes.h"

 // Returns the required length of the buffer to hold this structure
int MessageContainer::createNew(ID* server, ID* channel, ID* author, std::string message) {
    this->serverID = *server;
    this->channelID = *channel;
    this->author = *author;
    this->message = message;
    // +1 for null terminator
    this->messageLength = message.length();
    return sizeof(DataTypes) + UUID_BYTE_SIZE*4 + sizeof(int) + this->messageLength;
}

void MessageContainer::serialize(unsigned char* serializedData) {
    int offset = 0;

    // DataType
    DataTypes datatype = DataTypes::MESSAGETYPE;
    memcpy(serializedData+offset, &datatype, sizeof(DataTypes));
    offset += sizeof(DataTypes);

    // MessageID
    memcpy(serializedData+offset, this->messageID.getRaw(), UUID_BYTE_SIZE);
    offset += UUID_BYTE_SIZE;

    // ServerID
    memcpy(serializedData+offset, this->serverID.getRaw(), UUID_BYTE_SIZE);
    offset += UUID_BYTE_SIZE;

    // ChannelID
    memcpy(serializedData+offset, this->channelID.getRaw(), UUID_BYTE_SIZE);
    offset += UUID_BYTE_SIZE;

    // Author
    memcpy(serializedData+offset, this->author.getRaw(), UUID_BYTE_SIZE);
    offset += UUID_BYTE_SIZE;

    // Message Length
    memcpy(serializedData+offset, &this->messageLength, sizeof(this->messageLength));
    offset += sizeof(this->messageLength);

    // Message
    memcpy(serializedData+offset, this->message.data(), this->messageLength);
    offset += this->messageLength;
}

MessageContainer* MessageContainer::deserialize(unsigned char* data) {
    MessageContainer* newMessage = new MessageContainer;

    // The dataType has been removed by now
    int offset = sizeof(DataTypes);

    // MessageID
    newMessage->messageID = ID::fromBytes(data+offset);
    offset += UUID_BYTE_SIZE;

    // ServerID
    newMessage->serverID = ID::fromBytes(data+offset);
    offset += UUID_BYTE_SIZE;

    // ChannelID
    newMessage->channelID = ID::fromBytes(data+offset);
    offset += UUID_BYTE_SIZE;

    // Author
    newMessage->author = ID::fromBytes(data+offset);
    offset += UUID_BYTE_SIZE;

    // Message Length
    memcpy(&newMessage->messageLength, data+offset, sizeof(newMessage->messageLength));
    offset += sizeof(newMessage->messageLength);

    // Message
    std::string s( reinterpret_cast<char const*>( data+offset), newMessage->messageLength);
    newMessage->message = s;
    offset += newMessage->messageLength;
    return newMessage;
}

ID* MessageContainer::getMessageID() {
    return &this->messageID;
}

ID* MessageContainer::getServerID() {
    return &this->serverID;
}

ID* MessageContainer::getChannelID() {
    return &this->channelID;
}
ID* MessageContainer::getAuthor() {
    return &this->author;
}
std::string MessageContainer::getMessage() {
    return this->message;
}