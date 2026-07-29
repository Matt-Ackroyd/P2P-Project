#include "FileHandler.h"
#include "PrimaryClient.h"
#include "DatabaseConnection.h"

FileHandler::FileHandler()
{
}

void FileHandler::downloadFile(std::string fileID)
{
    this->incomingFiles[fileID] = new DownloadingFile(fileID);
}

void FileHandler::manageFiles()
{
    // Send Download Requests after a delay
    for (auto& [id, file]: this->incomingFiles) {
        if (!file->hostsDiscovered && file->downloadStarted + std::chrono::milliseconds(hostClaimDelay) < std::chrono::system_clock::now()) {
            file->sendDownloadRequests();
        }
    }

    // Manage outgoing files 
    for (OutgoingFile* file: this->outgoingFiles) {
        if (file->getRecipient()->connection.outgoingBuffer.size() < maxOutgoingPackets) {
            file->sendNextPacket();
        }
    }
}

void FileHandler::onFilePacketRecieved()
{
    
}







DownloadingFile::DownloadingFile(std::string fileID)
{
    PrimaryClient* client = PrimaryClient::getInstance();
    this->fileInfo = DatabaseConnection::getFileIndicatorFromDB(fileID);

    

    Server* server = client->getServer(fileInfo->getServerID());
    
    // Ask each user in the server if they are hosting this file
    for (std::string userid: server->onlineUsers) {
        UDPConnection remoteConnection = client->getUser(userid)->connection;

        Request request(fileInfo->getServerID(), fileID, DataTypes::FILE_HOST_CLAIM);
        remoteConnection.sendEncrypted(request.getData(), request.getDataLen());
    }
}

DownloadingFile::~DownloadingFile()
{
    delete this->fileInfo;
}

void DownloadingFile::sendDownloadRequests()
{
    if (avaliableHosts.size() > 0) {
        Request req(this->fileInfo->getServerID(), this->fileInfo->getFileID(), DataTypes::FILETYPE);
        this->avaliableHosts[0]->connection.sendEncrypted(req.getData(), req.getDataLen());
        this->hostsDiscovered = true;
    }
}

std::string DownloadingFile::getFileID()
{
    return this->fileID;
}

void DownloadingFile::addHost(RemoteUser* host)
{
    std::lock_guard<std::mutex> lock(mtx);
    this->avaliableHosts.emplace_back(host);

}

void DownloadingFile::RemoveHost(RemoteUser* host)
{
    std::lock_guard<std::mutex> lock(mtx);
    // TODO
}







void OutgoingFile::sendNextPacket()
{
    // Check if file exists and attempt to open it
    std::filesystem::path filePath(this->fileInfo->getLocalFilePath());
    
    if(!std::filesystem::exists(filePath)) {
        return;
    }

    std::ifstream file(filePath, std::ios::binary);

    if (!file.is_open()) {
        return;
    }

    // Read data into the buffer
    file.seekg(this->lastByteSent);

    char buffer[this->fileSizePerPacket];
    file.read(buffer, this->fileSizePerPacket);
    int bytesRead = file.gcount();

    file.close();
    
    // Send the File Data
    FileContainer a(fileInfo->getFileID(), lastByteSent, (unsigned char*)buffer, bytesRead);
    recipient->connection.sendEncrypted(a.getData(), a.getDataLen());

    // Update the last byte sent
    this->lastByteSent += bytesRead;


    // Check if this is the last packet & remove this object if it is

}

RemoteUser* OutgoingFile::getRecipient()
{
    return this->recipient;
}
