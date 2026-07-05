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
    std::string messageID;
    std::string serverID;
    std::string channelID;
    std::string author;
    std::string message;
    int messageLength;
    
    
public:
    int createNew(std::string server, std::string channel, std::string author, std::string message, std::string messageID = ""); // Returns the required length of the buffer to hold this structure

    void serialize(unsigned char* serializedData);
    static MessageContainer* deserialize(unsigned char* data);

    std::string* getMessageID();
    std::string* getServerID();
    std::string* getChannelID();
    std::string* getAuthor();
    std::string getMessage();
};

// a class to contain file data along side its ID
class FileContainer {
private:
    std::string fileID;
    int byteLocation; // Could use a better name (it means which byte of the file is this packet starting at)
    int datalen;
    unsigned char* data;
public:
    int createNew(std::string id, int byteLocation, unsigned char* data, int datalenth);
    void serialize(unsigned char* serializedData);
    static FileContainer deserialize(unsigned char* serializedData);
};

// a class containing meta data about a file
class FileIndicator {
private:
    std::string fileID;
    int fileSize;
    std::string relativeFileLocation;
    // File Signiture

    // Will not be serilized as this is ment for the original person who uploaded the file so we dont copy to the relitive path
    std::string localFileLocation;
public:
    int createNew(std::string id, int fileSize, std::string path);
    void serialize(unsigned char* serializedData);
    static FileIndicator deserialize(unsigned char* serializedData);
};



class ServerContainer {
    
};