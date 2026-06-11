#include "DataTypes.h"

 // Returns the required length of the buffer to hold this structure
int MessageContainer::createNew(UUID* channel, UUID* author, char* message, int msgLen) {
    this->messageID.GenerateNewID();
    this->channelID = channel;
    this->author = author;
    this->message = message;
    this->messageLength = msgLen;
    return UUID_BYTE_SIZE*3 + sizeof(int) + this->messageLength;
}

void MessageContainer::serialize(unsigned char* serializedData) {
    int offset = 0;

    // MessageID
    memcpy(serializedData+offset, this->messageID.getRaw(), UUID_BYTE_SIZE);
    offset += UUID_BYTE_SIZE;

    // ChannelID
    memcpy(serializedData+offset, this->channelID->getRaw(), UUID_BYTE_SIZE);
    offset += UUID_BYTE_SIZE;

    // Author
    memcpy(serializedData+offset, this->author->getRaw(), UUID_BYTE_SIZE);
    offset += UUID_BYTE_SIZE;

    // Message Length
    memcpy(serializedData+offset, &this->messageLength, sizeof(this->messageLength));
    offset += sizeof(this->messageLength);

    // Message
    memcpy(serializedData+offset, this->message, this->messageLength);
    offset += this->messageLength;
}

MessageContainer MessageContainer::deserialize(unsigned char* data) {
    MessageContainer newMessage;
    int offset = 0;

    // MessageID
    newMessage.messageID.set(data+offset);
    offset += UUID_BYTE_SIZE;

    // ChannelID
    newMessage.channelID->set(data+offset);
    offset += UUID_BYTE_SIZE;

    // Author
    newMessage.author->set(data+offset);
    offset += UUID_BYTE_SIZE;

    // Message Length
    memcpy(&newMessage.messageLength, data+offset, sizeof(newMessage.messageLength));
    offset += sizeof(newMessage.messageLength);

    // Message
    newMessage.message[newMessage.messageLength];
    memcpy(newMessage.message, data+offset, newMessage.messageLength);
    offset += newMessage.messageLength;
    return newMessage;
}

UUID* MessageContainer::getMessageID() {
    return &this->messageID;
}
UUID* MessageContainer::getChannel() {
    return this->channelID;
}
UUID* MessageContainer::getAuthor() {
    return this->author;
}
char* MessageContainer::getMessage() {
    return this->message;
}