#pragma once
#include "ID.h"

class Server;
class TextChannel;
class RemoteUser;

enum DataTypes {
    EMPTY,
    MESSAGETYPE,
    FILETYPE,
    FILE_INDICATOR,
    SERVER,
    TEXT_CHANNEL,
    VOICE_CHANNEL,
    USER,
    JOIN_REQUEST,
    REQUEST,
    SYNC,
    DELETED
};

class Container {
protected: 
    DataTypes datatype;
    int offset = sizeof(DataTypes);
    unsigned char* data = nullptr;
    int datalen;
    

public:
    Container(DataTypes datatype, int datalen) {
        this->datatype = datatype;
        this->datalen = sizeof(DataTypes) + datalen;

        if (datatype != DataTypes::EMPTY) {
            this->data = new unsigned char[this->datalen];
            // DataType
            memcpy(this->data, &datatype, sizeof(DataTypes));
        }
    }
    ~Container() { 
        delete[] this->data;
        this->data = nullptr;
    }

    unsigned char* getData() {return this->data;}
    int getDataLen() {return this->datalen;}
};


class Request : public Container {
private:
    std::string serverID;
    std::string requestedID;
    DataTypes requestedDatatype;
    void serialize();

    Request(DataTypes dataTypeOfThisObject, std::string serverID, std::string requestedID, DataTypes requestedDatatype);
public:
    Request(std::string serverID, std::string requestedID, DataTypes requestedDatatype);
    static Request deserialize(unsigned char* serializedData);

    DataTypes getRequestedDatatype();
    std::string getServerID();
    std::string getRequestedID();

    static void onRequest(unsigned char *decryptedData, RemoteUser *requestee);
};



// class to contain infomation about a message as well as the message itself
class MessageContainer : public Container{
private:
    std::string messageID;
    std::string serverID;
    std::string channelID;
    std::string author;
    std::string message;
    int messageLength;
    void serialize();
    
public:
    MessageContainer(DataTypes datatype, std::string server, std::string channel, std::string author, std::string message, std::string messageID = ""); 
    static MessageContainer* deserialize(unsigned char* data);

    std::string* getMessageID();
    std::string* getServerID();
    std::string* getChannelID();
    std::string* getAuthor();
    std::string getMessage();

    static void onRequest(std::string serverID, std::string id, RemoteUser* requestee);
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


class JoinRequest : public Container{
    std::string invitation;
    int contactAddress;
    short int contactPort;
    bool requiresRelay;

    JoinRequest(std::string givenInvitation, int contactAddress, short int contactPort, bool requiresRelay);
    void serialize();

public:
    JoinRequest(DataTypes datatype, std::string givenInvitation, int contactAddress, short int contactPort, bool requiresRelay);
    static JoinRequest deserialize(unsigned char* serializedData);

    std::string getInvitation();
    int getContactAddress();
    short int getContactPort();
    bool getRelayRequired();
};


class ServerContainer : public Container {
    std::string serverID;
    void serialize();
    // MoreToCome
    ServerContainer(std::string id);
public:
    ServerContainer(DataTypes datatype, Server* server);
    static ServerContainer deserialize(unsigned char* serializedData);

    std::string getServerID();
};


class TextChannelContainer : public Container {
    std::string serverID;
    std::string channelID;
    // MoreToCome
    TextChannelContainer(std::string serverid, std::string channelid);
    void serialize();
public:

    TextChannelContainer(DataTypes datatype, TextChannel* channel);
    
    static TextChannelContainer deserialize(unsigned char* serializedData);

    std::string getServerID();
    std::string getChannelID();
};

class UserContainer : public Container{
    std::string userID;
    std::string serverID;
    int contactAddress;
    short int contactPort;
    bool requiresRelay;

    void serialize();
    UserContainer(std::string userID, std::string serverID, int contactAddress, short int contactPort, bool requiresRelay);
public:
    UserContainer(DataTypes datatype, RemoteUser* user, Server* server);
    static UserContainer deserialize(unsigned char* serializedData);

    std::string getUserID();
    std::string getServerID();
    int getContactAddress();
    short int getContactPort();
    bool getRelayRequired();
};