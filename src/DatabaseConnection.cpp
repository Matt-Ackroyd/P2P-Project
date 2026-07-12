#include "DatabaseConnection.h"

std::mutex DatabaseConnection::mtx;


void DatabaseConnection::startup() {
    std::lock_guard<std::mutex> lock(mtx);

    sqlite3* db;
    int exit = 0;
    exit = sqlite3_open(DATABASE_NAME, &db);
    std::string sql = "CREATE TABLE Users("
                      "userID BLOB(16) PRIMARY KEY    NOT NULL, "
                      "username        TEXT,"
                      "contactAddress  INT(4)       NOT NULL, "
                      "contactPort     INT(2)       NOT NULL, "
                      "relayRequired   BOOLEAN      NOT NULL, "
                      "secret          BLOB(32),"
                      "UNIQUE(userID));";
    char* messaggeError;
    exit = sqlite3_exec(db, sql.c_str(), NULL, 0, &messaggeError);


    sql = "CREATE TABLE Servers("
            "serverID BLOB(16) PRIMARY KEY    NOT NULL);";
    exit = sqlite3_exec(db, sql.c_str(), NULL, 0, &messaggeError);  

    sql = "CREATE TABLE TextChannels("
            "channelID BLOB(16) PRIMARY KEY        NOT NULL, "
            "serverID  BLOB(16) NOT NULL,"
            "FOREIGN KEY(serverID) REFERENCES Servers(serverID)"
            ");";
    exit = sqlite3_exec(db, sql.c_str(), NULL, 0, &messaggeError);

    sql = "CREATE TABLE Messages("
            "messageID  BLOB(16)    PRIMARY KEY NOT NULL, "
            "channelID  BLOB(16)    NOT NULL,"
            "authorID   BLOB(16)    NOT NULL,"
            "contents   TEXT        NOT NULL,"
            "timestamp  UNSIGNED BIG INT         NOT NULL,"
            "FOREIGN KEY(authorID) REFERENCES Users(userID),"
            "FOREIGN KEY(channelID) REFERENCES Channels(channelID)"
        "); ";

    exit = sqlite3_exec(db, sql.c_str(), NULL, 0, &messaggeError);

    sql = "CREATE TABLE ServerUsers("
            "serverID BLOB(16), "
            "userID BLOB(16)," 
            "FOREIGN KEY(serverID) REFERENCES Servers(serverID),"
            "FOREIGN KEY(userID) REFERENCES Users(userID)"
        ");";
    exit = sqlite3_exec(db, sql.c_str(), NULL, 0, &messaggeError);

    sql = "CREATE TABLE Invitations("
            "invitationCode BLOB(16) PRIMARY KEY        NOT NULL, "
            "serverID  BLOB(16) NOT NULL,"
            "FOREIGN KEY(serverID) REFERENCES Servers(serverID)"
            ");";
    exit = sqlite3_exec(db, sql.c_str(), NULL, 0, &messaggeError);

    // Foren key table for Roles & User Role Links to servers

    sqlite3_close_v2(db);

}

void DatabaseConnection::addUserToDB(RemoteUser* user) {
    std::lock_guard<std::mutex> lock(mtx);
    sqlite3* db;
    int exit = 0;
    exit = sqlite3_open(DATABASE_NAME, &db);
    char* errMsg;
    std::string sql = "INSERT INTO Users (userID, username, contactAddress, contactPort, relayRequired, secret) VALUES (?, ?, ?, ?, ?, ?);";

    sqlite3_stmt* stmt; // will point to prepared stamement object
    exit = sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, nullptr);      
                        
    // UUID
    unsigned char uuid[UUID_BYTE_SIZE];
    ID::BytesFromString(*user->getID(), uuid);
    sqlite3_bind_blob(stmt, 1, (char*)uuid, UUID_BYTE_SIZE, nullptr);
    sqlite3_bind_text(stmt, 2, user->Username.c_str(), user->Username.length(), SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, user->contactAddress);
    sqlite3_bind_int(stmt, 4, user->contactPort);
    sqlite3_bind_int(stmt, 5, user->requiresRelay);
    sqlite3_bind_blob(stmt, 6, user->connection.getSharedSecret(), SHAW_256_HASH_SIZE, nullptr);

    int ret = sqlite3_step(stmt);
    
    sqlite3_finalize(stmt);

    sqlite3_close_v2(db);
}

void DatabaseConnection::getUsersFromDB() {
    //std::lock_guard<std::mutex> lock(mtx);
    sqlite3* db;
    int exit;
    exit = sqlite3_open(DATABASE_NAME, &db);
    char* errMsg;
    std::string sql("SELECT * FROM Users");

    sqlite3_stmt* stmt; // will point to prepared stamement object
    sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, nullptr);

    while(int ret = sqlite3_step(stmt) == SQLITE_ROW) {
        std::string id = ID::stringFromBytes((unsigned char*) sqlite3_column_blob(stmt, 0));

        RemoteUser* user = new RemoteUser(id);
        user->Username = (char*)sqlite3_column_text(stmt, 1);
        user->contactAddress = sqlite3_column_int(stmt, 2);
        user->contactPort = sqlite3_column_int(stmt, 3);
        user->requiresRelay = sqlite3_column_int(stmt, 4);
        

        if (sqlite3_column_type(stmt, 5) != SQLITE_NULL) {
            unsigned char* secret = new unsigned char[SHAW_256_HASH_SIZE];
            memcpy(secret, (unsigned char*)sqlite3_column_blob(stmt, 5), SHAW_256_HASH_SIZE);
            user->connection.setSharedSecret(secret);
        }

        PrimaryClient::getInstance()->loadUser(user);
    }
    sqlite3_finalize(stmt);

    sqlite3_close_v2(db);
}

void DatabaseConnection::addServerToDB(Server* server) {
    std::lock_guard<std::mutex> lock(mtx);
    sqlite3* db;
    int exit = 0;
    exit = sqlite3_open(DATABASE_NAME, &db);
    char* errMsg;
    std::string sql = "INSERT INTO Servers (serverID) VALUES (?);";

    sqlite3_stmt* stmt; // will point to prepared stamement object
    exit = sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, nullptr);      
                        
    // UUID
    unsigned char uuid[UUID_BYTE_SIZE];
    ID::BytesFromString(*server->getID(), uuid);
    sqlite3_bind_blob(stmt, 1, (char*)uuid, UUID_BYTE_SIZE, nullptr);

    int ret = sqlite3_step(stmt);
    
    sqlite3_finalize(stmt);
}

void DatabaseConnection::getServersFromDB() {
    //std::lock_guard<std::mutex> lock(mtx);
    sqlite3* db;
    int exit = 0;
    exit = sqlite3_open(DATABASE_NAME, &db);
    char* errMsg;
    std::string sql("SELECT * FROM Servers");

    sqlite3_stmt* stmt; // will point to prepared stamement object
    sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, nullptr);

    while(int ret = sqlite3_step(stmt) == SQLITE_ROW) {
        std::string id = ID::stringFromBytes((unsigned char*) sqlite3_column_blob(stmt, 0));

        Server* server = new Server(id);

        PrimaryClient::getInstance()->loadServer(server);
    }
    sqlite3_finalize(stmt);

    sqlite3_close_v2(db);
}

void DatabaseConnection::addTextChannelToDB(Server* server, TextChannel* channel) {
    std::lock_guard<std::mutex> lock(mtx);
    sqlite3* db;
    int exit = 0;
    exit = sqlite3_open(DATABASE_NAME, &db);
    char* errMsg;
    std::string sql = "INSERT INTO TextChannels (channelID, serverID) VALUES (?, ?);";

    sqlite3_stmt* stmt; // will point to prepared stamement object
    exit = sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, nullptr);      
                        
    // UUID
    unsigned char channeluuid[UUID_BYTE_SIZE];
    ID::BytesFromString(*channel->getID(), channeluuid);
    sqlite3_bind_blob(stmt, 1, (char*)channeluuid, UUID_BYTE_SIZE, nullptr);

    unsigned char serveruuid[UUID_BYTE_SIZE];
    ID::BytesFromString(*server->getID(), serveruuid);
    sqlite3_bind_blob(stmt, 2, (char*)serveruuid, UUID_BYTE_SIZE, nullptr);

    int ret = sqlite3_step(stmt);
    
    sqlite3_finalize(stmt);

    sqlite3_close_v2(db);
}

void DatabaseConnection::getTextChannelsFromDB(Server* server) {
    //std::lock_guard<std::mutex> lock(mtx);
    sqlite3* db;
    int exit = 0;
    exit = sqlite3_open(DATABASE_NAME, &db);
    char* errMsg;
    std::string sql("SELECT * FROM TextChannels WHERE serverID = ?");

    sqlite3_stmt* stmt; // will point to prepared stamement object
    sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, nullptr);

    // Selects only Channels assosiated with this server
    unsigned char uuid[UUID_BYTE_SIZE];
    ID::BytesFromString(*server->getID(), uuid);
    sqlite3_bind_blob(stmt, 1, (char*)uuid, UUID_BYTE_SIZE, nullptr);

    while(int ret = sqlite3_step(stmt) == SQLITE_ROW) {
        std::string id = ID::stringFromBytes((unsigned char*) sqlite3_column_blob(stmt, 0));

        TextChannel* textchannel = new TextChannel(server, id);

        server->loadChannel(textchannel);
    }
    sqlite3_finalize(stmt);
    sqlite3_close_v2(db);
}

void DatabaseConnection::addMessageToDB(TextChannel* channel, MessageContainer* message) {
    std::lock_guard<std::mutex> lock(mtx);
    sqlite3* db;
    int exit = 0;
    exit = sqlite3_open(DATABASE_NAME, &db);
    char* errMsg;
    std::string sql = "INSERT INTO Messages (messageID, channelID, authorID, contents, timestamp) VALUES ("
        "?, ?, ?, ?, ?"
    ");";

    sqlite3_stmt* stmt; // will point to prepared stamement object
    exit = sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, nullptr);      
                        
    // UUID
    unsigned char messageuuid[UUID_BYTE_SIZE];
    ID::BytesFromString(*message->getMessageID(), messageuuid);
    sqlite3_bind_blob(stmt, 1, (char*)messageuuid, UUID_BYTE_SIZE, nullptr);      // MessageID

    unsigned char channeluuid[UUID_BYTE_SIZE];
    ID::BytesFromString(*channel->getID(), channeluuid);
    sqlite3_bind_blob(stmt, 2, (char*)channeluuid, UUID_BYTE_SIZE, nullptr);      // ChannelID

    unsigned char authoruuid[UUID_BYTE_SIZE];
    ID::BytesFromString(*message->getAuthor(), authoruuid);
    sqlite3_bind_blob(stmt, 3, (char*)authoruuid, UUID_BYTE_SIZE, nullptr);      // AuthorID


    sqlite3_bind_text(stmt, 4, message->getMessage().c_str(), message->getMessage().length()+1, SQLITE_TRANSIENT); // Message Contents
    sqlite3_bind_int64(stmt, 5, ID::getTimestamp(*message->getMessageID()));                              // TimeStamp
    

    int ret = sqlite3_step(stmt);
    
    sqlite3_finalize(stmt);

    sqlite3_close_v2(db);
}

void DatabaseConnection::getMessagesFromDB(TextChannel* channel, int amount) {
    //std::lock_guard<std::mutex> lock(mtx);
    sqlite3* db;
    int exit = 0;
    exit = sqlite3_open(DATABASE_NAME, &db);
    char* errMsg;
    std::string sql("SELECT * FROM Messages WHERE channelID = ? ORDER BY timestamp LIMIT ?;");

    sqlite3_stmt* stmt; // will point to prepared stamement object
    sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, nullptr);

    // Selects only messages assosiated with this channel
    unsigned char uuid[UUID_BYTE_SIZE];
    ID::BytesFromString(*channel->getID(), uuid);
    sqlite3_bind_blob(stmt, 1, (char*)uuid, UUID_BYTE_SIZE, nullptr);
    sqlite3_bind_int(stmt, 2, amount);

    while(int ret = sqlite3_step(stmt) == SQLITE_ROW) {
        std::string id = ID::stringFromBytes((unsigned char*) sqlite3_column_blob(stmt, 0));
        std::string author = ID::stringFromBytes((unsigned char*) sqlite3_column_blob(stmt, 2));
        std::string contents = (char*)sqlite3_column_text(stmt, 3);

        MessageContainer* message = new MessageContainer(DataTypes::EMPTY, *channel->getServer()->getID(), *channel->getID(), author, contents, id);

        channel->loadMessage(message);
    }
    sqlite3_finalize(stmt);

    sqlite3_close_v2(db);
}

MessageContainer DatabaseConnection::getMessageFromDB(std::string id) {
    //std::lock_guard<std::mutex> lock(mtx);
    sqlite3* db;
    int exit = 0;
    exit = sqlite3_open(DATABASE_NAME, &db);
    char* errMsg;
    std::string sql("SELECT (messageID, channelID, ServerID, authorID, contents) FROM Messages INNER JOIN TextChannels ON Messages.channelID=TextChannels.channelID WHERE messageID = ?");

    sqlite3_stmt* stmt; // will point to prepared stamement object
    sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, nullptr);

    // Selects only messages assosiated with this channel
    unsigned char uuid[UUID_BYTE_SIZE];
    ID::BytesFromString(id, uuid);
    sqlite3_bind_blob(stmt, 1, (char*)uuid, UUID_BYTE_SIZE, nullptr);

    MessageContainer message(EMPTY, "", "", "", "");
    if (int ret = sqlite3_step(stmt) == SQLITE_ROW) {
        std::string id = ID::stringFromBytes((unsigned char*) sqlite3_column_blob(stmt, 0));
        std::string channel = ID::stringFromBytes((unsigned char*) sqlite3_column_blob(stmt, 1));
        std::string server = ID::stringFromBytes((unsigned char*) sqlite3_column_blob(stmt, 2));
        std::string author = ID::stringFromBytes((unsigned char*) sqlite3_column_blob(stmt, 3));
        std::string contents = (char*)sqlite3_column_text(stmt, 4);

        message = MessageContainer(DataTypes::MESSAGETYPE, server, channel, author, contents, id);
    }
    sqlite3_finalize(stmt);
    sqlite3_close_v2(db);

    if (*message.getAuthor() == "") {
        throw std::runtime_error("Message doesn't exist");
    }

    return message;
}

void DatabaseConnection::addUserToServerDB(RemoteUser* user, Server* server) {
    std::lock_guard<std::mutex> lock(mtx);

    sqlite3* db;
    int exit = 0;
    exit = sqlite3_open(DATABASE_NAME, &db);
    char* errMsg;
    std::string sql = "INSERT INTO ServerUsers (serverID, userID) VALUES (?, ?);";

    sqlite3_stmt* stmt; // will point to prepared stamement object
    exit = sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, nullptr);      
                        
    // UUID
    unsigned char serveruuid[UUID_BYTE_SIZE];
    ID::BytesFromString(*server->getID(), serveruuid);
    sqlite3_bind_blob(stmt, 1, (char*)serveruuid, UUID_BYTE_SIZE, nullptr);      // ServerID

    unsigned char useruuid[UUID_BYTE_SIZE];
    ID::BytesFromString(*user->getID(), useruuid);
    sqlite3_bind_blob(stmt, 2, (char*)useruuid, UUID_BYTE_SIZE, nullptr);      // UserID

    
    int ret = sqlite3_step(stmt);
    
    sqlite3_finalize(stmt);

    sqlite3_close_v2(db);
}

void DatabaseConnection::getUsersInServerFromDB(Server* server) {
    //std::lock_guard<std::mutex> lock(mtx);

    sqlite3* db;
    int exit = 0;
    exit = sqlite3_open(DATABASE_NAME, &db);
    char* errMsg;
    std::string sql("SELECT userID FROM ServerUsers WHERE serverID = ?");

    sqlite3_stmt* stmt; // will point to prepared stamement object
    sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, nullptr);

    // Selects only users in this server
    unsigned char uuid[UUID_BYTE_SIZE];
    ID::BytesFromString(*server->getID(), uuid);
    sqlite3_bind_blob(stmt, 1, (char*)uuid, UUID_BYTE_SIZE, nullptr);
    
    while(sqlite3_step(stmt) == SQLITE_ROW) {
        std::string userid = ID::stringFromBytes((unsigned char*) sqlite3_column_blob(stmt, 0));

        server->loadUser(PrimaryClient::getInstance()->getUser(userid));
    }
    sqlite3_finalize(stmt);

    sqlite3_close_v2(db);
}

void DatabaseConnection::addServerInvitationToDB(std::string invitation, Server* server) {
    std::lock_guard<std::mutex> lock(mtx);
    
    sqlite3* db;
    int exit = 0;
    exit = sqlite3_open(DATABASE_NAME, &db);
    char* errMsg;
    std::string sql = "INSERT INTO Invitations (invitationCode, serverID) VALUES (?, ?);";

    sqlite3_stmt* stmt; // will point to prepared stamement object
    exit = sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, nullptr);      
                        
    // UUID
    unsigned char inviationCode[UUID_BYTE_SIZE];
    ID::BytesFromString(invitation, inviationCode);
    sqlite3_bind_blob(stmt, 1, (char*)inviationCode, UUID_BYTE_SIZE, nullptr);      

    unsigned char serverid[UUID_BYTE_SIZE];
    ID::BytesFromString(*server->getID(), serverid);
    sqlite3_bind_blob(stmt, 2, (char*)serverid, UUID_BYTE_SIZE, nullptr);

    int ret = sqlite3_step(stmt);
    
    sqlite3_finalize(stmt);

    sqlite3_close_v2(db);
}

std::string DatabaseConnection::getInvitationsServerFromDB(std::string invitationCode) {
    //std::lock_guard<std::mutex> lock(mtx);

    sqlite3* db;
    int exit = 0;
    exit = sqlite3_open(DATABASE_NAME, &db);
    char* errMsg;
    std::string sql("SELECT serverID FROM Invitations WHERE invitationCode = ? LIMIT 1;");

    sqlite3_stmt* stmt; // will point to prepared stamement object
    sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, nullptr);

    // Selects only messages assosiated with this channel
    unsigned char uuid[UUID_BYTE_SIZE];
    ID::BytesFromString(invitationCode, uuid);
    sqlite3_bind_blob(stmt, 1, (char*)uuid, UUID_BYTE_SIZE, nullptr);

    while(int ret = sqlite3_step(stmt) == SQLITE_ROW) {
        std::string serverid = ID::stringFromBytes((unsigned char*) sqlite3_column_blob(stmt, 0));

        sqlite3_finalize(stmt);
        sqlite3_close_v2(db);

        return serverid;
    }

    sqlite3_finalize(stmt);
    sqlite3_close_v2(db);

    return "";
}