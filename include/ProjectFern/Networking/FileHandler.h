#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <fstream>
#include "RemoteUser.h"
#include "DataTypes.h"



class DownloadingFile {
    std::string fileID;
    FileIndicator* fileInfo;
    std::unordered_set<std::string> avaliableHosts;

    std::mutex mtx;

public: 
    DownloadingFile(std::string fileID);
    ~DownloadingFile();

    std::chrono::time_point<std::chrono::_V2::system_clock, std::chrono::_V2::system_clock::duration> downloadStarted = std::chrono::system_clock::now();

    void sendDownloadRequests();

    std::string getFileID();
    void addHost(RemoteUser*);
    void RemoveHost(RemoteUser*);

    bool hostsDiscovered = false;
};

class OutgoingFile {
    RemoteUser* recipient;
    std::string fileID;
    FileIndicator* fileInfo;

    int lastByteSent = 0;
    int fileSizePerPacket = 4096;

public:
    OutgoingFile(std::string fileID, RemoteUser* recipient);
    ~OutgoingFile();

    void sendNextPacket();
    RemoteUser* getRecipient();
};




class FileHandler {
public: 
    FileHandler();
    std::unordered_map<std::string, DownloadingFile*> incomingFiles;
    std::unordered_set<OutgoingFile*> outgoingFiles;
    int maxOutgoingPackets = 30;
    int hostClaimDelay = 3000;

    // Adds a file to be downloaded
    void downloadFile(std::string fileID);
    void sendFile(std::string fileID, RemoteUser* recipient);

    // Sends Download Requests after a short Period inorder to give time for host claims to be returned & also manages outgoing files
    void manageFiles();


    static void onFilePacketRecieved(unsigned char* packetdata);

};