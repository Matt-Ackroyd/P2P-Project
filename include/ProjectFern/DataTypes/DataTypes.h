#pragma once
#include "ID.h"

enum DataTypes {
    MESSAGETYPE,
    FILETYPE,
    FILEINDICATOR
};

// class to contain infomation about a message as well as the message itself
class MessageContainer {
private:
    ID messageID;
    ID *channelID;
    ID *author;
    char* message;
    int messageLength;
    
    
public:
    int createNew(ID* channel, ID* author, char* message, int msgLen); // Returns the required length of the buffer to hold this structure

    void serialize(unsigned char* serializedData);
    static MessageContainer deserialize(unsigned char* data);

    ID* getMessageID();
    ID* getChannel();
    ID* getAuthor();
    char* getMessage();
};

// a class to contain file data along side its ID
class FileContainer {
private:
    ID fileID;
    int byteLocation; // Could use a better name (it means which byte of the file is this packet starting at)
    int datalen;
    unsigned char* data;
public:
    int createNew(ID id, int byteLocation, unsigned char* data, int datalenth);
    void serialize(unsigned char* serializedData);
    static FileContainer deserialize(unsigned char* serializedData);
};

// a class containing meta data about a file
class FileIndicator {
private:
    ID fileID;
    int fileSize;
    string relativeFileLocation;
    // File Signiture

    // Will not be serilized as this is ment for the original person who uploaded the file so we dont copy to the relitive path
    string localFileLocation;
public:
    int createNew(ID id, int fileSize, string path);
    void serialize(unsigned char* serializedData);
    static FileIndicator deserialize(unsigned char* serializedData);
};