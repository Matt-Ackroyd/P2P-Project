#include "ConfigLoader.h"

ConfigLoader* ConfigLoader::instancePtr = nullptr;
std::mutex ConfigLoader::mtx;

// Static method to get the Singleton instance
ConfigLoader* ConfigLoader::getInstance() {
    if (instancePtr == nullptr) {
        std::lock_guard<std::mutex> lock(mtx);
        if (instancePtr == nullptr) {
            instancePtr = new ConfigLoader();
        }
    }
    return instancePtr;
    
}


int ConfigLoader::WriteBinaryFile(std::string path, char* data, int datalen) {
    std::filesystem::path filepath(path);
    
    std::filesystem::path dirPath = filepath.parent_path();
    // if the directory doesn't exist create it
    if (!std::filesystem::exists(filepath.parent_path())) {
        std::filesystem::create_directories(dirPath);
    }

    std::ofstream file(filepath, std::ios::binary);
    if (file.is_open()) {
        // File IO
        file.write(data, datalen); 
    } else {
        // Return on failed operation
        return -1;
    }
    file.close();
    return 1;
}

int ConfigLoader::ReadBinaryFile(std::string path, char* data, int datalen) {
    std::filesystem::path filepath(path);
    
    std::ifstream file(filepath, std::ios::binary);
    if (file.is_open()) {
        // File IO
        file.read(data, datalen); 
    } else {
        // Return on failed operation
        return -1;
    }
    file.close();
    return 1;
}


void ConfigLoader::sqlitetest() {
    sqlite3* DB;
    int exit = 0;
    exit = sqlite3_open("example.db", &DB);
    std::string sql = "CREATE TABLE USERS("
                      "ID TEXT PRIMARY KEY    NOT NULL, "
                      "NAME            TEXT    NOT NULL, "
                      "ADRESS          INT    NOT NULL, "
                      "PORT            INT    NOT NULL, "
                      "SECRET          CHAR(32));";
    char* messaggeError;
    exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messaggeError);


    sql = "CREATE TABLE SERVERS("
            "ID TEXT PRIMARY KEY    NOT NULL, "
            "NAME            TEXT    NOT NULL, "
            "PORT            INT    NOT NULL, "
            "ADRESS          INT    NOT NULL, "
            "SECRET          CHAR(50));";
    exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messaggeError);  

    sql = "CREATE TABLE CHANNELS("
            "ID TEXT PRIMARY KEY        NOT NULL, "
            "SERVER TEXT FOREIGN KEY    NOT NULL, " // Holds the SERVER ID
            "SECRET                     CHAR(50));";
    exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messaggeError);

    sql = "CREATE TABLE MESSAGES("
            "ID TEXT PRIMARY KEY       NOT NULL, "
            "CHANNEL TEXT FOREIGN KEY  NOT NULL, " // Holds the CHANNEL ID that holds this message
            "CONTENTS           TEXT   NOT NULL"
            "AUTHOR             CHAR(50));";
    exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messaggeError);

    sql = "CREATE TABLE ServerUsers("
            "ServerId[ForeignKey], "
            "UserId[ForeignKey]" 
            ");";
    exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messaggeError);

    // Foren key table for invitations & Roles
}