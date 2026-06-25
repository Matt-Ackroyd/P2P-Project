#include "RelayServer.h"

// Adds a user to the data base with a hash of the salted password
// Requires a Encrypted Connection Beforehand 
void onRegisterUser(ID* userID, unsigned char* data, sockaddr_in* cliaddr) {
    // Data the password
    std::string password(reinterpret_cast<char const*>(data), PASSWORD_BYTE_SIZE);\

    // Create the path to the file
    std::filesystem::path path = std::filesystem::current_path();
    path.append(PATH_TO_USER_FILES);

    // If the directory doesn't exist yet create it
    if (!std::filesystem::exists(PATH_TO_USER_FILES)) {
        std::filesystem::create_directory(PATH_TO_USER_FILES);
    }

    // Sanitize the string
    if (userID->getString().find(".") != std::string::npos) {
        // Abort, there should never be a . symbol in the userID so its likely they are trying to insert this file outside of the correct dir
        return;
    }
    path.append(userID->getString());
    
    // Make sure that we dont overide an existing entry
    if (std::filesystem::exists(path)) {
        std::cout << "User Already exists\n";
        return;
    }    

    
    password += "StaltyFern";   // Salt the password
    
    // Hash
    char shaw256output[SHAW_256_HASH_SIZE];
    shaw256Hash((unsigned char*)password.c_str(), password.length()+1, (unsigned char*)shaw256output);

    std::ofstream file(path, std::ios::binary);
    if (file.is_open()) {
        // File IO
        file.write(shaw256output, SHAW_256_HASH_SIZE);   // Pass Hash
        
        // Connection Info
        char outgoingBuffer[CONNECTION_INFO_SIZE];
        memcpy(outgoingBuffer, &cliaddr->sin_addr.s_addr, sizeof(cliaddr->sin_addr.s_addr));               // Copy Addr
        memcpy(outgoingBuffer+sizeof(int), &cliaddr->sin_port, sizeof(cliaddr->sin_port));                 // Copy Port
        file.seekp(SHAW_256_HASH_SIZE);
        file.write(outgoingBuffer, CONNECTION_INFO_SIZE);
    }
    file.close();
}


// Updates the stored connection info of a given user, as long as the password hash works
// Requires a Encrypted Connection Beforehand 
void onUpdateUserConnectionInfo(ID* userID, unsigned char* data, sockaddr_in* cliaddr) {
    // Data contains the password 
    std::string givenPassword(reinterpret_cast<char const*>(data), PASSWORD_BYTE_SIZE);

    // Create the path to the file
    std::filesystem::path path = std::filesystem::current_path();
    path.append(PATH_TO_USER_FILES);

    // If the directory doesn't exist yet create it
    if (!std::filesystem::exists(PATH_TO_USER_FILES)) {
        std::filesystem::create_directory(PATH_TO_USER_FILES);
    }

    // Sanitize the string
    if (userID->getString().find(".") != std::string::npos) {
        // Abort, there should never be a . symbol in the userID so its likely they are trying to insert this file outside of the correct dir
        return;
    }
    path.append(userID->getString());

    // Make sure that this user exists
    if (!std::filesystem::exists(path)) {
        return;
    }    

    // Salt & hash the given password
    givenPassword += "StaltyFern";   
    char shaw256output[SHAW_256_HASH_SIZE];
    shaw256Hash((unsigned char*)givenPassword.c_str(), givenPassword.length()+1, (unsigned char*)shaw256output);

    std::ifstream readfile(path, std::ios::binary);
    if (readfile.is_open()) {
        // Compare given password hash with hash on record
        char passwordOnFile[SHAW_256_HASH_SIZE];
        readfile.read(passwordOnFile, SHAW_256_HASH_SIZE);
        if (strncmp(passwordOnFile, shaw256output, SHAW_256_HASH_SIZE) != 0) {     // if passwords DONT match
            return;                                               
        }
    }
    readfile.close();

    // We will only reach this point if the passwords match
    std::fstream writefile(path, std::ios::binary | std::ios::in | std::ios::out);
    if (writefile.is_open()) {
        char outgoingBuffer[CONNECTION_INFO_SIZE];
        memcpy(outgoingBuffer, &cliaddr->sin_addr.s_addr, sizeof(cliaddr->sin_addr.s_addr));               // Copy Addr
        memcpy(outgoingBuffer+sizeof(int), &cliaddr->sin_port, sizeof(cliaddr->sin_port));                 // Copy Port
        writefile.seekp(SHAW_256_HASH_SIZE);     // move the put pointer to the connection info section of the file
        writefile.write(outgoingBuffer, CONNECTION_INFO_SIZE);  
    }
    writefile.close();



}


void handleTcpConnection(SOCKTYPE clientSocket, sockaddr_in clientAddress) {
    char buffer[3000] = {0};
    recv(clientSocket, buffer, sizeof(buffer), 0);

    Packet handshakePacket(-1, PacketType::NONE, NULL);
    int datalen = handshakePacket.deserialize(buffer);

    // Return if this stream isn't requested to be encrypted
    if (handshakePacket.getPacketType() != PacketType::HANDSHAKE_REQUEST) {
        closesocket(clientSocket);
        return;
    }
    
    // Create Shared Secret for encryption/decryption
    unsigned char secret[SHAW_256_HASH_SIZE];
    if (ML_KEM_Handshake::onRequest(&handshakePacket, clientSocket, clientAddress, sizeof(clientAddress), NULL, secret) < 1) {
        return;
    }

    std::ofstream file("secret.bin", std::ios::binary);
    if (file.is_open()) {
        file.write((char*)secret, SHAW_256_HASH_SIZE);
    }
    file.close();

    // Reset the buffer and lisen for the clients real request
    buffer[3000] = {0};
    recv(clientSocket, buffer, sizeof(buffer), 0);

    Packet incomingPacket(-1, PacketType::NONE, NULL);
    datalen = incomingPacket.deserialize(buffer);

    // DECRYPTION
    // AAD Gen for the senderID and incoming length of the data
    unsigned char aad[UUID_BYTE_SIZE + sizeof(datalen)];
    memcpy(aad, incomingPacket.packetAuthorID.getRaw(), UUID_BYTE_SIZE);
    memcpy(aad+UUID_BYTE_SIZE, &datalen, sizeof(datalen));

    // Decrypt Here
    unsigned char output[datalen];
    if (symmetricDecryption((unsigned char*)incomingPacket.getData(), datalen, aad, sizeof(aad), incomingPacket.getTag(), 
            secret, incomingPacket.getIV(), AES_256_IV_LENGTH, output) < 1) {
        closesocket(clientSocket);
        return;
    }
    
    
    switch(incomingPacket.getPacketType()) {
        case PacketType::RELAY_REQUEST_USER_REGISTRATION:
            onRegisterUser(&incomingPacket.packetAuthorID, output, &clientAddress);
            break;
        case PacketType::RELAY_REQUEST_UPDATE_CONNECTION_INFO:
            onUpdateUserConnectionInfo(&incomingPacket.packetAuthorID, output, &clientAddress);
            break;
        default:
            std::cout << "Invalid Request\n";
    }

    closesocket(clientSocket);
}


void onUserConnectionInfoReqest(char* buffer, SOCKTYPE socketfd, sockaddr_in* cliaddr, socklen_t clientlen) {
    ID userID;
    userID.set((unsigned char*)buffer);

    // Check if given ID is in the database
    if (!std::filesystem::exists(PATH_TO_USER_FILES + userID.getString())) {
        sendto(socketfd, "No User Found", sizeof("No User Found"), 0, (struct sockaddr*)cliaddr, clientlen);
        return;
    }   

    char connection_buffer[CONNECTION_INFO_SIZE];
    std::ifstream file(PATH_TO_USER_FILES + userID.getString(), std::ios::binary);
    if (file.is_open()) {
        file.seekg(SHAW_256_HASH_SIZE); // Look at the connection info after the password hash
        file.read(connection_buffer, CONNECTION_INFO_SIZE);
    } else {
        return;
    }
    file.close();

    // Send the original requester the connection info of the requsted user
    sendto(socketfd, connection_buffer, CONNECTION_INFO_SIZE, 0, (struct sockaddr*)cliaddr, clientlen);


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

    char outgoingBuffer[CONNECTION_INFO_SIZE];
    memcpy(outgoingBuffer, &cliaddr->sin_addr.s_addr, sizeof(cliaddr->sin_addr.s_addr));               // Copy Addr
    memcpy(outgoingBuffer+sizeof(int), &cliaddr->sin_port, sizeof(cliaddr->sin_port));                 // Copy Port

    sendto(socketfd, outgoingBuffer, CONNECTION_INFO_SIZE, 0, (struct sockaddr*)&addrToInform, sizeof(addrToInform));
}

// returns a users connection info & sends them a notification with your connection information
// Does not require an Encrypted Connection
void UdpHandler(int udpPort) {
    char buffer[32]; 
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
        recvfrom(socketfd, buffer, 32,
            0, (struct sockaddr*)&cliaddr, &clientlen);
            
        onUserConnectionInfoReqest(buffer, socketfd, &cliaddr, clientlen);
        
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

    //udp
    std::thread UDPLoop(UdpHandler, port);

    SOCKTYPE tcpServerSocket = socket(AF_INET, SOCK_STREAM, 0);  

    // Reuse socket if on windows
    #ifdef _WIN32
        int optVal = 1;
        setsockopt(tcpServerSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&optVal, sizeof(optVal));
    #endif

    // Bind adress and port
    sockaddr_in tcpServerAddress;
    tcpServerAddress.sin_family = AF_INET;
    tcpServerAddress.sin_port = htons(port);
    tcpServerAddress.sin_addr.s_addr = INADDR_ANY;

    int a = bind(tcpServerSocket, (struct sockaddr*)&tcpServerAddress, sizeof(tcpServerAddress));
    std::cout << "TCP Bind Return: " << a << "\n";

    sockaddr_in clientAddress;
    int len;
    listen(tcpServerSocket, 10);
    // TCP Connection loop
    while (true) {
        SOCKTYPE clientSocket = accept(tcpServerSocket, (struct sockaddr*)&clientAddress, &len);
        
        
        std::thread* newThread = new std::thread(handleTcpConnection, clientSocket, clientAddress);
    }

    UDPLoop.join();
}