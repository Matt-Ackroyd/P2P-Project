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

