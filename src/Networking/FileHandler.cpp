#include "FileHandler.h"
#include "PrimaryClient.h"
#include "DatabaseConnection.h"

void FileHandler::downloadFile(std::string fileID)
{
    this->incomingFiles.emplace(DownloadingFile(fileID));
}

void FileHandler::manageFiles()
{
    // Send Download Requests after a delay


    // Manage outgoing files 
    for (OutgoingFile file: this->outgoingFiles) {
        if (file.getRecipient()->connection.outgoingBuffer.size() < maxOutgoingPackets) {
            file.sendNextPacket();
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
    Request req(this->fileInfo->getServerID(), this->fileInfo->getFileID(), DataTypes::FILETYPE);
    this->avaliableHosts[0]->connection.sendEncrypted(req.getData(), req.getDataLen());
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
}

RemoteUser* OutgoingFile::getRecipient()
{
    return this->recipient;
}
