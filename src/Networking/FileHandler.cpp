#include "FileHandler.h"
#include "PrimaryClient.h"
#include "DatabaseConnection.h"

FileHandler::DownloadingFile::DownloadingFile(std::string fileID)
{
    std::mutex mtx;
    PrimaryClient* client = PrimaryClient::getInstance();
    this->fileInfo = DatabaseConnection::getFileIndicatorFromDB(fileID);

    

    Server* server = client->getServer(fileInfo->getServerID());
    
    // Ask each user in the server if they are hosting this file
    for (auto& [userid, user]: server->knownUsers) {

        // If users online
        if (user->connection.connected) {
            Request request(fileInfo->getServerID(), fileID, DataTypes::FILE_HOST_CLAIM);
            user->connection.sendEncrypted(request.getData(), request.getDataLen());
        }
    }
}

FileHandler::DownloadingFile::~DownloadingFile()
{
    delete this->fileInfo;
}

void FileHandler::DownloadingFile::sendDownloadRequests()
{
    if (avaliableHosts.size() > 0) {
        Request req(this->fileInfo->getServerID(), this->fileInfo->getFileID(), DataTypes::FILETYPE);
        for (auto& hostid: this->avaliableHosts) {
            RemoteUser* host = PrimaryClient::getInstance()->getUser(hostid);
            host->connection.sendEncrypted(req.getData(), req.getDataLen());
            break;
        }
        this->hostsDiscovered = true;
    }
}

std::string FileHandler::DownloadingFile::getFileID()
{
    return this->fileID;
}

void FileHandler::DownloadingFile::addHost(RemoteUser* host)
{
    //std::lock_guard<std::mutex> lock(mtx);
    
    std::string id = *host->getID();
    this->avaliableHosts.emplace(id);

}

void FileHandler::DownloadingFile::RemoveHost(RemoteUser* host)
{
    //std::lock_guard<std::mutex> lock(mtx);
    std::string id = *host->getID();
    this->avaliableHosts.erase(id);
}





















FileHandler::OutgoingFile::OutgoingFile(std::string fileID, RemoteUser *recipient)
{
    this->fileID = fileID;
    this->recipient = recipient;
    this->fileInfo = DatabaseConnection::getFileIndicatorFromDB(fileID);
}

void FileHandler::OutgoingFile::sendNextPacket()
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

    char *buffer = new char[this->fileSizePerPacket];
    file.read(buffer, this->fileSizePerPacket);
    int bytesRead = file.gcount();

    file.close();
    
    // Send the File Data
    FileContainer* container = new FileContainer(fileInfo->getFileID(), lastByteSent, (unsigned char*)buffer, bytesRead);
    recipient->connection.sendEncrypted(container->getData(), container->getDataLen());
    delete container;

    // Update the last byte sent
    this->lastByteSent += bytesRead;


    // Check if this is the last packet & remove this object if it is
    if (bytesRead != fileSizePerPacket) {
        PrimaryClient::getInstance()->fileHandler->outgoingFiles.erase(this);
        delete this;
    }
}

RemoteUser* FileHandler::OutgoingFile::getRecipient()
{
    return this->recipient;
}

















FileHandler::FileHandler()
{
}

void FileHandler::downloadFile(std::string fileID)
{
    this->incomingFiles[fileID] = new DownloadingFile(fileID);
}

void FileHandler::sendFile(std::string fileID, RemoteUser* recipient)
{
    FileHandler::OutgoingFile* file = new FileHandler::OutgoingFile(fileID, recipient);
    this->outgoingFiles.emplace(file);
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
        if (outgoingFiles.empty()) {
            break;
        }
        if (file->getRecipient()->connection.numOfOutgoingPackets < maxOutgoingPackets) {
            file->sendNextPacket();
        }
    }
}

void FileHandler::onFilePacketRecieved(unsigned char* output)
{
    FileHandler* fileHandler = PrimaryClient::getInstance()->fileHandler;
    FileContainer filedata = FileContainer::deserialize(output);
    FileIndicator* fileInfo = DatabaseConnection::getFileIndicatorFromDB(filedata.getFileID());

    // If this file isn't being requested then ignore it
    if (!fileHandler->incomingFiles.contains(filedata.getFileID())) {
        return;
    }

    

    std::filesystem::path path(fileInfo->getLocalFilePath());
    std::ofstream file(path, std::ios::binary | std::ios::app | std::ios::out );
    
    if (!file.is_open()) {
        return;
    }

    file.seekp(filedata.getByteLocation());
    file.write((char*)filedata.getFileData(), filedata.getFileDatalen());
    int filesize = file.tellp();
    file.close();
    
    
    // If this is the last of the file
    if (filesize >= fileInfo->getFileSize()) {
        DownloadingFile* file = fileHandler->incomingFiles[filedata.getFileID()];
        fileHandler->incomingFiles.erase(filedata.getFileID());
        delete file;
    }

    delete fileInfo;
}