#include "DataTypes.h"

 // Returns the required length of the buffer to hold this structure
int MessageContainer::createNew(std::string server, std::string channel, std::string author, std::string message, std::string messageid) {
    this->serverID = server;
    this->channelID = channel;
    this->author = author;
    this->message = message;
    // +1 for null terminator
    this->messageLength = message.length();

    this->messageID = ID::clean(messageid);
    
    return sizeof(DataTypes) + UUID_BYTE_SIZE*4 + sizeof(int) + this->messageLength;
}

void MessageContainer::serialize(unsigned char* serializedData) {
    unsigned char uuid[UUID_BYTE_SIZE];
    int offset = 0;

    // DataType
    DataTypes datatype = DataTypes::MESSAGETYPE;
    memcpy(serializedData+offset, &datatype, sizeof(DataTypes));
    offset += sizeof(DataTypes);

    // MessageID
    ID::BytesFromString(this->messageID, uuid);
    memcpy(serializedData+offset, uuid, UUID_BYTE_SIZE);
    offset += UUID_BYTE_SIZE;

    // ServerID
    ID::BytesFromString(this->serverID, uuid);
    memcpy(serializedData+offset, uuid, UUID_BYTE_SIZE);
    offset += UUID_BYTE_SIZE;

    // ChannelID
    ID::BytesFromString(this->channelID, uuid);
    memcpy(serializedData+offset, uuid, UUID_BYTE_SIZE);
    offset += UUID_BYTE_SIZE;

    // Author
    ID::BytesFromString(this->author, uuid);
    memcpy(serializedData+offset, uuid, UUID_BYTE_SIZE);
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
    newMessage->messageID = ID::stringFromBytes(data+offset);
    offset += UUID_BYTE_SIZE;

    // ServerID
    newMessage->serverID = ID::stringFromBytes(data+offset);
    offset += UUID_BYTE_SIZE;

    // ChannelID
    newMessage->channelID = ID::stringFromBytes(data+offset);
    offset += UUID_BYTE_SIZE;

    // Author
    newMessage->author = ID::stringFromBytes(data+offset);
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