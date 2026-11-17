#pragma once

#include "DataTypes.h"
#include <vector>

#define MAX_SYNC_REQUEST 200

class SyncRequest : public Container{
private:
    std::string syncID;
    DataTypes objectType;
    std::string serverID;
    int rangeStart;
    int rangeEnd;
    void serialize();
    SyncRequest(std::string syncID, std::string serverID, DataTypes objectType, int rangeStart, int rangeEnd);

public:
    SyncRequest(std::string serverID, DataTypes objectType, int rangeStart, int rangeEnd);
    static SyncRequest deserialize(unsigned char* serializedData);

    std::string getSyncID();
    DataTypes getObjectType();
    std::string getserverID();
    int getStartRange();
    int getEndRange();
    static void onSyncRequest(unsigned char *decryptedData, RemoteUser *requestee);
};

class SyncResponse : public Container {
private:
    std::string syncID;
    std::vector<std::string> listOfIDs;
    void serialize();

    
public:
    SyncResponse(DataTypes dataType, int sizeOfStructure, std::string syncID, std::vector<std::string> listOfIDs);

    static SyncResponse generateResponse(SyncRequest* request);
    static SyncResponse deserialize(unsigned char *serializedData);

    std::string getSyncID();
    std::vector<std::string> getListOfIDs();


    static void onSyncResponse(unsigned char *decryptedData, RemoteUser *sender);
    static std::vector<std::string> getSyncFromDB(std::string sql, std::string serverID, int rangeStart, int rangeEnd);
    static bool doesDBcontain(std::string sql, std::string id, std::string serverID);


};