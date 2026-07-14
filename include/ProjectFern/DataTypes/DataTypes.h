#pragma once
#include "ID.h"

#define SIGNITURE_SIZE 32

class Server;
class TextChannel;
class RemoteUser;

enum DataTypes {
    EMPTY,
    MESSAGETYPE,
    FILETYPE,
    FILE_INDICATOR,
    FILE_HOST_CLAIM,
    SERVER,
    TEXT_CHANNEL,
    VOICE_CHANNEL,
    USER,
    JOIN_REQUEST,
    ADD_OR_MODIFY_REQUEST,
    DELETE_REQUEST,
    SYNC_REQUEST,
    SYNC_RESPONSE,
    DELETED
};

class Container {
protected: 
    DataTypes datatype;
    int offset = sizeof(DataTypes);
    unsigned char* data = nullptr;
    int datalen;
    

public:
    Container(DataTypes type, int len) {
        this->datatype = type;
        this->datalen = sizeof(DataTypes) + len;

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
    
    bool destoryOnSend;
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
class FileContainer : public Container{
private:
    std::string fileID;
    int byteLocation; // Could use a better name (it means which byte of the file is this packet starting at)
    int datalen;
    unsigned char* fileData = nullptr;
    void serialize();

public:
    FileContainer(std::string id, int byteLocation, unsigned char* data, int datalenth, DataTypes type = FILETYPE);
    ~FileContainer();

    static FileContainer deserialize(unsigned char* serializedData);

    std::string getFileID();
    int getByteLocation();
    int getDatalen();
    unsigned char* getData();
};

// a class containing meta data about a file
class FileIndicator : public Container{
private:
    std::string fileID;
    std::string serverID;
    int fileSize;
    std::string relativeFileLocation;
    // File Signiture
    unsigned char signature[SIGNITURE_SIZE];

    // Will not be serilized as this is ment for the original person who uploaded the file so we dont copy to the relitive path
    std::string localFileLocation;
    void serialize();
public:
    FileIndicator(std::string relativePath, int fileSize, std::string serverID, 
        unsigned char* signature, std::string localPath = "", std::string id = "", DataTypes type = DataTypes::FILE_INDICATOR);
    static FileIndicator deserialize(unsigned char* serializedData);

    static void onAddRequest(std::string serverID, std::string fileID, RemoteUser *requestee);
    static void onRemoveRequest(std::string serverID, std::string fileID, RemoteUser *requestee);
};  


class FileHostClaim : public Container {
    std::string fileID;

public:
    FileHostClaim(std::string fileID, DataTypes type = DataTypes::FILE_HOST_CLAIM);

    std::string deserialize(unsigned char* serializedData);
    static void onAddRequest();
    static void onRemoveRequest();
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