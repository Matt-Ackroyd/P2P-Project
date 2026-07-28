#include "Sync.h"
#include "DatabaseConnection.h"
#include "PrimaryClient.h"

SyncResponse::SyncResponse(DataTypes dataType, int sizeOfStructure, std::string syncID, std::vector<std::string> listOfIDs) :
Container(dataType, sizeOfStructure) {
    this->syncID = syncID;
    this->listOfIDs = listOfIDs;
}

void SyncResponse::serialize() {
    unsigned char uuid[UUID_BYTE_SIZE];    

    // syncID
    ID::BytesFromString(this->syncID, uuid);
    memcpy(data+offset, uuid, UUID_BYTE_SIZE);
    offset += UUID_BYTE_SIZE;

    // number of elements
    int numberOfIds = listOfIDs.size();
    memcpy(data+offset, &numberOfIds, sizeof(int));
    offset += sizeof(int);

    // List of ID's
    for(std::string id: this->listOfIDs) {
        ID::BytesFromString(id, uuid);
        memcpy(data+offset, uuid, UUID_BYTE_SIZE);
        offset += UUID_BYTE_SIZE;
    }
}


SyncResponse SyncResponse::deserialize(unsigned char* serializedData) {
    int offset = sizeof(DataTypes);

    // syncID
    std::string syncID = ID::stringFromBytes(serializedData+offset);
    offset += UUID_BYTE_SIZE;

    // number of elements
    int numberOfIds;
    memcpy(&numberOfIds, serializedData+offset, sizeof(numberOfIds));
    offset += sizeof(numberOfIds);

    // ids
    std::vector<std::string> listOfIDs;

    for (int i = 0; i < numberOfIds; i++) {
        std::string id = ID::stringFromBytes(serializedData+offset);
        listOfIDs.emplace_back(id);
        offset += UUID_BYTE_SIZE;
    }

    return SyncResponse(DataTypes::EMPTY, 0, syncID, listOfIDs);
}



SyncResponse SyncResponse::generateResponse(SyncRequest* request) {
    std::string serverID = request->getserverID();
    int rangeStart = request->getStartRange();
    int rangeEnd = request->getEndRange();
    
    std::vector<std::string> ids;
    switch(request->getObjectType()) {
        case DataTypes::TEXT_CHANNEL:
            ids = getSyncFromDB("SELECT channelID FROM TextChannels WHERE serverID = ? limit ?, ?", serverID, rangeStart, rangeEnd);
            break;
        case DataTypes::VOICE_CHANNEL:
            break;
        case DataTypes::MESSAGETYPE: //TODO Replace the ServerID with a parrent ID and do this on a channel basis checking if the requesting user has acsess to said channel
            ids = getSyncFromDB("SELECT messageID FROM Messages INNER JOIN TextChannels ON Messages.channelID=TextChannels.channelID "
                "WHERE serverID = ? limit ?, ?", serverID, rangeStart, rangeEnd);
            break;
        case DataTypes::FILE_INDICATOR:
            ids = getSyncFromDB("SELECT fileID FROM Files WHERE serverID = ? limit ?, ?", serverID, rangeStart, rangeEnd);
            break;
        case DataTypes::USER:
            ids = getSyncFromDB("SELECT userID FROM ServerUsers WHERE serverID = ? limit ?, ?", serverID, rangeStart, rangeEnd);
        case DataTypes::DELETED:
            break;
    }

    int size = ids.size()*UUID_BYTE_SIZE + UUID_BYTE_SIZE + sizeof(int);
    SyncResponse response = SyncResponse(DataTypes::SYNC_RESPONSE, size, request->getSyncID(), ids);
    response.serialize();
    return response;
}

std::vector<std::string> SyncResponse::getSyncFromDB(std::string sql, std::string serverID, int rangeStart, int rangeEnd) {
    sqlite3* db;
    int exit = 0;
    exit = sqlite3_open(DATABASE_NAME, &db);
    char* errMsg;

    sqlite3_stmt* stmt; // will point to prepared stamement object
    sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, nullptr);

    // Server Select
    unsigned char uuid[UUID_BYTE_SIZE];
    ID::BytesFromString(serverID, uuid);
    sqlite3_bind_blob(stmt, 1, (char*)uuid, UUID_BYTE_SIZE, nullptr);

    sqlite3_bind_int(stmt, 2, rangeStart); // Start Range
    sqlite3_bind_int(stmt, 3, rangeEnd); // End Range
    
    std::vector<std::string> ids;
    while(sqlite3_step(stmt) == SQLITE_ROW) { // TODO ADD A CHECK FOR USER PERMS TO BE SEND THIS CHANNEL
        std::string id = ID::stringFromBytes((unsigned char*) sqlite3_column_blob(stmt, 0));
        ids.emplace_back(id);
    }
    sqlite3_finalize(stmt);

    sqlite3_close_v2(db);

    return ids;
}

std::string SyncResponse::getSyncID() {
    return syncID;
}
std::vector<std::string> SyncResponse::getListOfIDs() {
    return listOfIDs;
}




void SyncResponse::onSyncResponse(unsigned char* decryptedData, RemoteUser* sender) {
    SyncResponse response = SyncResponse::deserialize(decryptedData);

    // If you didn't request this then ignore it
    if (!sender->connection.syncRequests.contains(response.getSyncID())) {
        return;
    }
    SyncRequest* syncrequest = sender->connection.syncRequests[response.getSyncID()];
    sender->connection.syncRequests.erase(response.getSyncID());

    std::string serverID = syncrequest->getserverID();

    // If this request was full then send another one 
    if (response.getListOfIDs().size() >= MAX_SYNC_REQUEST) {
        SyncRequest* newRequest = new SyncRequest(serverID, syncrequest->getObjectType(), syncrequest->getEndRange(), syncrequest->getEndRange()+MAX_SYNC_REQUEST);
        sender->connection.sendEncrypted(newRequest->getData(), newRequest->getDataLen());
    }

    for (std::string id: response.getListOfIDs()) {
        bool knownID;
        switch(syncrequest->getObjectType()) {
            case DataTypes::TEXT_CHANNEL:
                knownID = doesDBcontain("SELECT * FROM TextChannels WHERE channelID = ? AND serverID = ?", id, serverID);
                break;
            case DataTypes::VOICE_CHANNEL:
                break;
            case DataTypes::MESSAGETYPE: 
                knownID = doesDBcontain("SELECT * FROM Messages INNER JOIN TextChannels ON Messages.channelID=TextChannels.channelID "
                    "WHERE MessageID = ? AND serverID = ?", id, serverID);
                break;
            case DataTypes::FILE_INDICATOR:
                knownID = doesDBcontain("SELECT * FROM Files WHERE fileID = ? AND serverID = ?", id, serverID);
                break;
            case DataTypes::USER:
                knownID = doesDBcontain("SELECT * FROM serverUsers WHERE userID = ? AND serverID = ?", id, serverID);
                break;
            case DataTypes::DELETED:
                break;
        }

        // If this is an ID we dont have, request it
        if (!knownID) {
            Request request(serverID, id, syncrequest->getObjectType());
            sender->connection.sendEncrypted(request.getData(), request.getDataLen());
            //sender->connection.listOfRequestedIDs.emplace(id);
        }
    }
    
    delete syncrequest;
}



bool SyncResponse::doesDBcontain(std::string sql, std::string id, std::string serverID) {
    sqlite3* db;
    int exit = 0;
    exit = sqlite3_open(DATABASE_NAME, &db);
    char* errMsg;

    sqlite3_stmt* stmt; // will point to prepared stamement object
    sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, nullptr);

    // id Selection
    unsigned char uuid[UUID_BYTE_SIZE];
    ID::BytesFromString(id, uuid);
    sqlite3_bind_blob(stmt, 1, (char*)uuid, UUID_BYTE_SIZE, nullptr);

    // ServerID selection
    unsigned char serveruuid[UUID_BYTE_SIZE];
    ID::BytesFromString(serverID, serveruuid);
    sqlite3_bind_blob(stmt, 2, (char*)serveruuid, UUID_BYTE_SIZE, nullptr);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        sqlite3_finalize(stmt);
        sqlite3_close_v2(db);
        return true;
    }
    sqlite3_finalize(stmt);
    sqlite3_close_v2(db);
    return false;
}