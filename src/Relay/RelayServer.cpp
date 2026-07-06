#include "RelayServer.h"


void RelayServer::onUserConnectionInfoReqest(Packet* packet, SOCKTYPE socketfd, sockaddr_in* cliaddr, socklen_t clientlen) {
    std::string userID;
    userID = ID::stringFromBytes((unsigned char*)packet->getData());

    std::filesystem::path path = UserPath(userID);
    path.append("connectionInfo.bin");

    // Check if given ID is in the database
    if (!std::filesystem::exists(path)) {
        throw std::runtime_error("User Isn't Registered\n");
    }   

    char connection_buffer[CONNECTION_INFO_SIZE];
    std::ifstream file(path, std::ios::binary);
    if (file.is_open()) {
        file.read(connection_buffer, CONNECTION_INFO_SIZE);
    } else {
        return;
    }
    file.close();

    // Send the original requester the connection info of the requsted user
    Packet packet1(-1, PacketType::RELAY_USER_INFO, &userID);
    int packet1Len = packet1.serialize(connection_buffer, CONNECTION_INFO_SIZE, NULL, NULL);
    sendto(socketfd, packet1.getData(), packet1Len, 0, (struct sockaddr*)cliaddr, clientlen);


    // Inform the requested user that this one wishes to establish a connection
    // Read addr and port from file
    int addr;
    short int port; 
    memcpy(&addr, connection_buffer, sizeof(addr));
    memcpy(&port, connection_buffer+sizeof(addr), sizeof(port));

    sockaddr_in addrToInform; 
    socklen_t addrToInformLen = sizeof(addrToInform);
    memset(&addrToInform, 0, addrToInformLen);
    addrToInform.sin_family    = AF_INET;           // IPV4
    addrToInform.sin_addr.s_addr = addr;            // IP adress
    addrToInform.sin_port = port;                   // Port

    // int a = ntohs(addrToInform.sin_port);
    // char *ip = inet_ntoa(addrToInform.sin_addr);

    char outgoingBuffer[CONNECTION_INFO_SIZE];
    memcpy(outgoingBuffer, &cliaddr->sin_addr.s_addr, sizeof(cliaddr->sin_addr.s_addr));               // Copy Addr
    memcpy(outgoingBuffer+sizeof(int), &cliaddr->sin_port, sizeof(cliaddr->sin_port));                 // Copy Port

    // Encapsulate the data in a packet, (the author of the packet is the id of the owner of the connection info)
    Packet packet2(-1, PacketType::RELAY_USER_INFO, &packet->packetAuthorID);
    int packet2Len = packet2.serialize(outgoingBuffer, CONNECTION_INFO_SIZE, NULL, NULL);
    sendto(socketfd, packet2.getData(), packet2Len, 0, (struct sockaddr*)&addrToInform, sizeof(addrToInform));
}




void RelayServer::EstablishSharedSecret(Packet* handshakePacket, SOCKTYPE socketfd, sockaddr_in cliaddr) {
    unsigned char secret[SHAW_256_HASH_SIZE];
    Packet* returnPacket = ML_KEM_Handshake::onRequest(handshakePacket, NULL, secret, -1, PacketType::RELAY_HANDSHAKE_RESPONSE);

    // Store Secret
    std::filesystem::path path = UserPath(handshakePacket->packetAuthorID);
    path.append("handshakeHash.bin");

    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return;
    }
    file.write((char*)secret, SHAW_256_HASH_SIZE);
    file.close();

    // Send Return Info
    int a = sendto(socketfd, returnPacket->getData(), returnPacket->getPacketlength(), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
    delete returnPacket;
}

void RelayServer::onRequestRegistration(Packet* incomingPacket, SOCKTYPE socketfd, sockaddr_in cliaddr) {
    // Read Handshake Secret
    std::filesystem::path path = UserPath(incomingPacket->packetAuthorID);
    std::filesystem::path handshakeHashPath = path;
    handshakeHashPath.append("handshakeHash.bin");

    std::ifstream file(handshakeHashPath, std::ios::binary);
    if (!file.is_open()) {
        return;
    }
    unsigned char secret[SHAW_256_HASH_SIZE];
    file.read((char*)secret, SHAW_256_HASH_SIZE);
    file.close();

    // AAD Creation
    int datalen = incomingPacket->getDataLength();
    unsigned char aad[UUID_BYTE_SIZE + sizeof(datalen)];
    unsigned char uuid[UUID_BYTE_SIZE];
    ID::BytesFromString(incomingPacket->packetAuthorID, uuid);
    memcpy(aad, uuid, UUID_BYTE_SIZE);
    memcpy(aad+UUID_BYTE_SIZE, &datalen, sizeof(datalen));

    // Decrypt Here
    unsigned char output[datalen];
    if (symmetricDecryption((unsigned char*)incomingPacket->getData(), datalen, aad, sizeof(aad), incomingPacket->getTag(), 
            secret, incomingPacket->getIV(), AES_256_IV_LENGTH, output) < 1) {
        return;
    }

    // chech if this is the first time registering:
    std::filesystem::path passwordHashPath = path;
    passwordHashPath.append("passwordHash.bin");
    if (!std::filesystem::exists(passwordHashPath)) {
        CreatePasswordHashFile(path, output);
        UpdateConnectionInfo(path, output, cliaddr);
    } else {
        UpdateConnectionInfo(path, output, cliaddr);
    }
}

void RelayServer::CreatePasswordHashFile(std::filesystem::path path, unsigned char* passwordData) {
    // Hash & salt
    unsigned char shaw256output[SHAW_256_HASH_SIZE];
    SaltAndHash(passwordData, shaw256output);   

    // Write to file
    std::filesystem::path passwordHashPath = path;
    passwordHashPath.append("passwordHash.bin");

    std::ofstream file(passwordHashPath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("CreatePasswordHashFile: Failed To Open File\n");
    }

    file.write((char*)shaw256output, SHAW_256_HASH_SIZE);
    file.close();
}

void RelayServer::UpdateConnectionInfo(std::filesystem::path path, unsigned char* passwordData, sockaddr_in cliaddr) {
    std::filesystem::path passwordHashPath = path;
    std::filesystem::path connectionInfoPath = path;
    passwordHashPath.append("passwordHash.bin");
    connectionInfoPath.append("connectionInfo.bin");

    // Hash & salt
    unsigned char shaw256output[SHAW_256_HASH_SIZE];
    SaltAndHash(passwordData, shaw256output);  

    // Open and compare Password Hashes
    std::ifstream readfile(passwordHashPath, std::ios::binary | std::ios::in);
    if (!readfile.is_open()) {
        throw std::runtime_error("UpdateConnectionInfo: Failed To Open PasswordHash\n");
    }

    // Password Match
    char passwordOnFile[SHAW_256_HASH_SIZE];
    readfile.read(passwordOnFile, SHAW_256_HASH_SIZE);
    if (strncmp(passwordOnFile, (char*)shaw256output, SHAW_256_HASH_SIZE) != 0) {     // if passwords DONT match
        throw std::runtime_error("UpdateConnectionInfo: Passwords Dont Match");      
        return;                                         
    }
    readfile.close();

    // Write Connection Info
    std::fstream writefile(connectionInfoPath, std::ios::binary | std::ios::out);
    if (!writefile.is_open()) {
        throw std::runtime_error("UpdateConnectionInfo: Failed To Open ConectionInfo\n");
    }

    char outgoingBuffer[CONNECTION_INFO_SIZE];
    memcpy(outgoingBuffer, &cliaddr.sin_addr.s_addr, sizeof(cliaddr.sin_addr.s_addr));               // Copy Addr
    memcpy(outgoingBuffer+sizeof(int), &cliaddr.sin_port, sizeof(cliaddr.sin_port));                 // Copy Port

    writefile.write(outgoingBuffer, CONNECTION_INFO_SIZE);  
    writefile.close();
}

void RelayServer::SaltAndHash(unsigned char* input, unsigned char* output) {
    std::string password(reinterpret_cast<char const*>(input), PASSWORD_BYTE_SIZE);
    password += "StaltyFern";   // Salt the password
    shaw256Hash((unsigned char*)password.c_str(), password.length()+1, (unsigned char*)output);
}

std::filesystem::path RelayServer::UserPath(std::string userID) {
    // Create the path to the file
    std::filesystem::path path = std::filesystem::current_path();
    path.append(PATH_TO_USER_FILES);

    // If the directory doesn't exist yet create it
    if (!std::filesystem::exists(PATH_TO_USER_FILES)) {
        std::filesystem::create_directories(PATH_TO_USER_FILES);
    }

    // Sanitize the string
    if (userID.find(".") != std::string::npos) {
        // Abort, there should never be a . symbol in the userID so its likely they are trying to insert this file outside of the correct dir
        throw std::runtime_error("Impossible UserID");
    }
    path.append(userID + "/");

    if (!std::filesystem::exists(path)) {
        std::filesystem::create_directories(path);
    }
    return path;
}


// returns a users connection info & sends them a notification with your connection information
// Does not require an Encrypted Connection
void RelayServer::UdpHandler(int udpPort) {
    char buffer[3000]; 
    struct sockaddr_in servaddr, cliaddr; 
    socklen_t clientlen = sizeof(cliaddr);

    SOCKTYPE socketfd = socket(AF_INET, SOCK_DGRAM, 0); 
    
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Socket: IPV4, Any connection, PORTNumber
    servaddr.sin_family    = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(udpPort);
    

    int a = bind(socketfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    std::cout << "UDP Bind Return: " << a << "\n";

    while (true) {
        int packetlen = recvfrom(socketfd, buffer, 3000,
            0, (struct sockaddr*)&cliaddr, &clientlen);

        if (packetlen > Packet::MIN_PACKET_SIZE) {
            try {
                Packet packet(-1, PacketType::NONE, NULL);
                int datalen = packet.deserialize(buffer);

                if (packet.getPacketType() == PacketType::RELAY_REGISTER_REQUEST) {
                    onRequestRegistration(&packet, socketfd, cliaddr);

                } else if (packet.getPacketType() == PacketType::HANDSHAKE_REQUEST) {
                    EstablishSharedSecret(&packet, socketfd, cliaddr);
                }

                // Make sure we are getting the right packet
                else if (packet.getPacketType() == PacketType::RELAY_USER_INFO && datalen == UUID_BYTE_SIZE) {    
                    RelayServer::onUserConnectionInfoReqest(&packet, socketfd, &cliaddr, clientlen);
                }
            } catch (std::runtime_error e) {
                std::cout << "UDP Exception Caught: " << e.what();
            }
            catch (...) // catch-all handler
            {
                std::cout << "We caught an exception of an undetermined type\n";
            }
        }
    }
}


int main(int argc, char *argv[]) {
    int port = 0;
    if (argc < 2) {
        port = 7777;
    } else {
        port = atoi(argv[1]);
    }

    // Windows Startup code
    #ifdef _WIN32
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2,2), &wsaData);
    #endif

    
    RelayServer::UdpHandler(port);
}