#include "DataTypes.h"
#include "DatabaseConnection.h"
#include "FileHandler.h"

FileHostClaim::FileHostClaim(std::string fileID, bool hasFile, DataTypes type) : Container(type, sizeof(bool) + UUID_BYTE_SIZE) {
    this->hasFile = hasFile;

    if (type == DataTypes::FILE_HOST_CLAIM) {
        ID::BytesFromString(this->fileID, this->data+this->offset);
        this->offset += UUID_BYTE_SIZE;

        memcpy(this->data+this->offset, &this->hasFile, sizeof(bool));
    }
}

FileHostClaim FileHostClaim::deserialize(unsigned char* serializedData) {
    // The dataType has been removed by now
    int offset = sizeof(DataTypes);

    // fileID
    std::string fileID = ID::stringFromBytes(serializedData+offset);
    offset += UUID_BYTE_SIZE;

    bool hasFile = false;
    memcpy(&hasFile, serializedData+offset, sizeof(bool));
    offset += sizeof(bool);

    return FileHostClaim(fileID, hasFile, EMPTY);
}

bool FileHostClaim::hostHasFile()
{
    return this->hasFile;
}

std::string FileHostClaim::getFileID()
{
    return this->fileID;
}

void FileHostClaim::onRequest(std::string serverID, std::string fileID, RemoteUser *requestee) { 
    // Get the path of this file
    sqlite3* db;
    int exit = 0;
    exit = sqlite3_open(DATABASE_NAME, &db);
    std::string sql("SELECT localPath "
                    "FROM Files "
                    "WHERE fileID = ? AND serverID = ?");

    sqlite3_stmt* stmt; // will point to prepared stamement object
    sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, nullptr);

    // Selects this file
    unsigned char uuid[UUID_BYTE_SIZE];
    ID::BytesFromString(fileID, uuid);
    sqlite3_bind_blob(stmt, 1, (char*)uuid, UUID_BYTE_SIZE, nullptr);

    unsigned char serveruuid[UUID_BYTE_SIZE];
    ID::BytesFromString(serverID, serveruuid);
    sqlite3_bind_blob(stmt, 2, (char*)serveruuid, UUID_BYTE_SIZE, nullptr);

    std::string localPath = "";

    int ret = sqlite3_step(stmt);
    if (ret == SQLITE_ROW) {
        localPath = (char*)sqlite3_column_text(stmt, 1); 
    }
    sqlite3_finalize(stmt);
    sqlite3_close_v2(db);


    // If the file exists
    bool response = false;
    if (std::filesystem::exists(std::filesystem::path(localPath)) && !localPath.empty()) {
        response = true;
    }

    
    PrimaryClient* client = PrimaryClient::getInstance();
    Server* server = client->getServer(serverID);
    if (server == NULL) {
        return;
    }

    if (!server->knownUsers.contains(*requestee->getID())) { // If they dont belong to this server dont send them anything
        return;
    }

    FileHostClaim claim(fileID, response);
    requestee->connection.sendEncrypted(claim.getData(), claim.getDataLen());

}



void FileHostClaim::onReceived(unsigned char* output, RemoteUser* sender) {
    FileHostClaim claim = FileHostClaim::deserialize(output);

    // Make Sure We Requested this
    if (PrimaryClient::getInstance()->fileHandler->incomingFiles.contains(claim.fileID)) {
        return;
    }

    // Make sure that this user has the file
    if (!claim.hostHasFile()) {
        return;
    }

    PrimaryClient::getInstance()->fileHandler->incomingFiles[claim.fileID].addHost(sender);

}