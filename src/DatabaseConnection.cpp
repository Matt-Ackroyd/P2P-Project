#include "DatabaseConnection.h"


void DatabaseConnection::sqlitetest() {
    sqlite3* DB;
    int exit = 0;
    exit = sqlite3_open(DATABASE_NAME, &DB);
    std::string sql = "CREATE TABLE Users("
                      "UserID BLOB(16) PRIMARY KEY    NOT NULL, "
                      "username        TEXT,"
                      "contactAddress  TEXT         NOT NULL, "
                      "contactPort     INT          NOT NULL, "
                      "relay           BOOLEAN      NOT NULL, "
                      "secret          BLOB(32));";
    char* messaggeError;
    exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messaggeError);


    sql = "CREATE TABLE Servers("
            "serverID BLOB(16) PRIMARY KEY    NOT NULL);";
    exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messaggeError);  

    sql = "CREATE TABLE Channels("
            "channelID BLOB(16) PRIMARY KEY        NOT NULL, "
            "type      TEXT,"
            "server    BLOB(16) NOT NULL,"
            "FOREIGN KEY(server) REFERENCES Servers(ServerId)"
            ");";
    exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messaggeError);

    sql = "CREATE TABLE Messages("
            "messageID  BLOB(16)    PRIMARY KEY NOT NULL, "
            "contents   TEXT        NOT NULL,"
            "author     BLOB        NOT NULL,"
            "channel    BLOB        NOT NULL,"
            "FOREIGN KEY(author) REFERENCES Users(UserID),"
            "FOREIGN KEY(channel) REFERENCES Channels(ChannelID)"
        "); ";

    exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messaggeError);

    sql = "CREATE TABLE ServerUsers("
            "ServerId BLOB(16), "
            "UserId BLOB(16)," 
            "FOREIGN KEY(ServerId) REFERENCES Servers(ServerId),"
            "FOREIGN KEY(UserId) REFERENCES Users(UserId)"
        ");";
    exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messaggeError);

    // Foren key table for invitations & Roles


    sqlite3_close(DB);
}

void DatabaseConnection::addUserToDB(RemoteUser* user) {
    sqlite3* db;
    int exit = sqlite3_open(DATABASE_NAME, &db);
    char* errMsg;
    std::string sql = "INSERT INTO Users (UserID, username, contactAddress, contactPort, relay, secret) VALUES ("
        "?, ?, ?, ?, ?, ?"
    ");";

    sqlite3_stmt* stmt; // will point to prepared stamement object
    exit = sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, nullptr);      
                        
    // UUID
    sqlite3_bind_blob(stmt, 1, (char*)user->getID()->getRaw(), UUID_BYTE_SIZE, nullptr);
    sqlite3_bind_text(stmt, 2, user->Username.c_str(), user->Username.length(), SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, user->contactAdress.c_str(), user->contactAdress.length(), SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, user->contactPort);
    sqlite3_bind_int(stmt, 5, user->requiresRelay);
    sqlite3_bind_blob(stmt, 6, user->connection.getSharedSecret(), SHAW_256_HASH_SIZE, nullptr);

    int ret = sqlite3_step(stmt);
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void DatabaseConnection::getUsersFromDB() {
    sqlite3* db;
    int exit = sqlite3_open(DATABASE_NAME, &db);
    char* errMsg;
    std::string sql("SELECT * FROM Users");

    sqlite3_stmt* stmt; // will point to prepared stamement object
    sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, nullptr);

    while(int ret = sqlite3_step(stmt) == SQLITE_ROW) {
        ID id = ID::fromBytes((unsigned char*) sqlite3_column_blob(stmt, 0));

        RemoteUser* user = new RemoteUser(&id);
        user->Username = (char*)sqlite3_column_text(stmt, 1);
        user->contactAdress = (char*)sqlite3_column_text(stmt, 2);
        user->contactPort = sqlite3_column_int(stmt, 3);
        user->requiresRelay = sqlite3_column_int(stmt, 4);
        user->connection.setSharedSecret((unsigned char*)sqlite3_column_blob(stmt, 5));

        PrimaryClient::getInstance()->loadUser(user);
    }

    sqlite3_close(db);
}

