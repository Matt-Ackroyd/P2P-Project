#pragma once
#include <mutex>
#include <unordered_map>
#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>

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

    int WriteBinaryFile(std::string path, char* data, int datalen);
    int ReadBinaryFile(std::string path, char* data, int datalen);
    
};