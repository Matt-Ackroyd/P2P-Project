#pragma once
#include "UUID.h"

enum DataTypes {
    MESSAGETYPE,
    FILETYPE,
    FILEINDICATOR
};


class MessageContainer
{
private:
    UUID messageID;
    UUID *channelID;
    UUID *author;
    char* message;
    int messageLength;
    
    
public:
    int createNew(UUID* channel, UUID* author, char* message, int msgLen); // Returns the required length of the buffer to hold this structure

    void serialize(unsigned char* serializedData);
    static MessageContainer deserialize(unsigned char* data);

    UUID* getMessageID();
    UUID* getChannel();
    UUID* getAuthor();
    char* getMessage();
};


class FileContainer
{
private:
    UUID fileID;
public:
    FileContainer(/* args */);
    ~FileContainer();
};
