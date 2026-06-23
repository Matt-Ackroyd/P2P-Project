#include "RelayServer.h"

// Adds a user to the data base with a hash of the salted password
// Requires a Encrypted Connection Beforehand 
void onRegisterUser(ID* userID, unsigned char* data) {
    // Data the password
    std::string password(reinterpret_cast<char const*>(data), PASSWORD_SIZE);

    
    // Make sure that we dont overide an existing entry
    if (std::filesystem::exists(PATH_TO_USER_FILES + userID->getString())) {
        std::cout << "User Already exists\n";
        return;
    }    
    
    password += "StaltyFern";   // Salt the password
    
    // Hash
    unsigned char shaw256output[SHAW_256_HASH_SIZE];
    shaw256Hash((unsigned char*)password.c_str(), password.length()+1, shaw256output);

    std::ofstream file(PATH_TO_USER_FILES + userID->getString(), std::ios::binary);
    if (file.is_open()) {
        // File IO
        file.write((char*)shaw256output, SHAW_256_HASH_SIZE);   // Pass Hash
        //file.write();                                         // Connection Info
    }
    file.close();
}


// Updates the stored connection info of a given user, as long as the password hash works
// Requires a Encrypted Connection Beforehand 
void onUpdateUserConnectionInfo(ID* userID, unsigned char* data) {
    // Data contains the password 
    std::string givenPassword(reinterpret_cast<char const*>(data), PASSWORD_SIZE);

    // Make sure that this user exists
    if (!std::filesystem::exists(PATH_TO_USER_FILES + userID->getString())) {
        return;
    }    

    // Salt & hash the given password
    givenPassword += "StaltyFern";   
    unsigned char shaw256output[SHAW_256_HASH_SIZE];
    shaw256Hash((unsigned char*)givenPassword.c_str(), givenPassword.length()+1, shaw256output);

    std::fstream file(PATH_TO_USER_FILES + userID->getString(), std::ios::binary);
    if (file.is_open()) {
        // Compare given password hash with hash on record
        char passwordOnFile[SHAW_256_HASH_SIZE];
        file.read(passwordOnFile, SHAW_256_HASH_SIZE);
        if (strcmp(passwordOnFile, (char*)shaw256output)) {     // if passwords match
            file.seekp(SHAW_256_HASH_SIZE);                     // move the put pointer to the connection info section of the file
            //file.write()                                        // Write connection info
        }
        //file.write(); // Connection Info
    }
    file.close();


    

}


void handleTcpConnection(SOCKTYPE clientSocket, sockaddr_in clientAddress) {
    char buffer[1024] = {0};
    recv(clientSocket, buffer, sizeof(buffer), 0);

    Packet *incomingPacket = new Packet(-1, PacketType::NONE);
    int datalen = incomingPacket->deserialize(buffer);

    // Return if this stream isn't requested to be encrypted
    if (incomingPacket->getPacketType() != PacketType::HANDSHAKE_REQUEST) {
        return;
    }
    
    // Create Shared Secret for encryption/decryption
    unsigned char* secret;
    secret = onML_KEM_HandshakeRequest(incomingPacket, clientSocket, &clientAddress, sizeof(clientAddress));

    // Free memory
    delete incomingPacket;
    // Reset the buffer and lisen for the clients real request
    buffer[1024] = {0};
    recv(clientSocket, buffer, sizeof(buffer), 0);

    incomingPacket = new Packet(-1, PacketType::NONE);
    datalen = incomingPacket->deserialize(buffer);

    // DECRYPTION
    // AAD Gen for the senderID and incoming length of the data
    unsigned char aad[UUID_BYTE_SIZE + sizeof(datalen)];
    memcpy(aad, incomingPacket->packetAuthorID.getRaw(), UUID_BYTE_SIZE);
    memcpy(aad+UUID_BYTE_SIZE, &datalen, sizeof(datalen));

    // Decrypt Here
    unsigned char output[datalen];
    if (!symmetricDecryption((unsigned char*)incomingPacket->getData(), datalen, aad, sizeof(aad), incomingPacket->getTag(), 
            secret, incomingPacket->getIV(), AES_256_IV_LENGTH, output)) {
        exit(1);
    }
    
    
    switch(incomingPacket->getPacketType()) {
        case PacketType::RELAY_REQUEST_USER_REGISTRATION:
            onRegisterUser(&incomingPacket->packetAuthorID, output);
        case PacketType::RELAY_REQUEST_UPDATE_CONNECTION_INFO:
            onUpdateUserConnectionInfo(&incomingPacket->packetAuthorID, output);
        default:
            std::cout << "Invalid Request\n";
    }

    delete incomingPacket;
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
    memcpy(outgoingBuffer+sizeof(int), &cliaddr->sin_port, sizeof(cliaddr->sin_port));          // Copy Port

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
    std::cout << " Bind Return: " << a << "\n";

    while (true) {
        recvfrom(socketfd, buffer, 32,
            0, (struct sockaddr*)&cliaddr, &clientlen);
            
        onUserConnectionInfoReqest(buffer, socketfd, &cliaddr, clientlen);
        
    }
}






int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cout << "Incorrect Comand line args: program.exe <tcpPort> <udpPort> \n";
    }
    int tcpPort = atoi(argv[1]);
    int udpPort = atoi(argv[2]);

    // Windows Startup code
    #ifdef _WIN32
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2,2), &wsaData);
    #endif

    SOCKTYPE tcpsServerSocket = socket(AF_INET, SOCK_STREAM, 0);  

    // Reuse socket if on windows
    #ifdef _WIN32
        int optVal = 1;
        setsockopt(tcpsServerSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&optVal, sizeof(optVal));
    #endif

    // Bind adress and port
    sockaddr_in tcpServerAddress;
    tcpServerAddress.sin_family = AF_INET;
    tcpServerAddress.sin_port = htons(tcpPort);
    tcpServerAddress.sin_addr.s_addr = INADDR_ANY;

    bind(tcpsServerSocket, (struct sockaddr*)&tcpServerAddress, sizeof(tcpServerAddress));

    //udp
    std::thread UDPLoop(UdpHandler, udpPort);

    // Connection loop
    while (true) {
        listen(tcpsServerSocket, 10);

        sockaddr_in clientAddress;
        int len;
        SOCKTYPE clientSocket = accept(tcpsServerSocket, (struct sockaddr*)&clientAddress, &len);
        
        std::thread newThread(handleTcpConnection, clientSocket, clientAddress);
    }

    UDPLoop.join();
}