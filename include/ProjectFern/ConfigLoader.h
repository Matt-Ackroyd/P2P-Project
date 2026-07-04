#pragma once
#include <mutex>
#include <unordered_map>
#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sqlite3.h>


class ConfigLoader {
private:
    // Static pointer to the Singleton instance
    static ConfigLoader* instancePtr;
    // Mutex to ensure thread safety
    static std::mutex mtx;
    // Private Constructor
    ConfigLoader() {}
public:
    // Deleting the copy constructor to prevent copies
    ConfigLoader(const ConfigLoader& obj) = delete;
    static ConfigLoader* getInstance();

    static int WriteBinaryFile(std::string path, char* data, int datalen);
    static int ReadBinaryFile(std::string path, char* data, int datalen);
    
    static void sqlitetest();
};