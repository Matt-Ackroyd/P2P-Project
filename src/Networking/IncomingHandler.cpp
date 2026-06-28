#include "IncomingHandler.h"

IncomingHandler::IncomingHandler(int ReceivingPort) {
    this->IncomingHandlerThread = std::thread(&IncomingHandler::incomingStartup, this, ReceivingPort);
}


// Setup
void IncomingHandler::incomingStartup(int ReceivingPort)
{
    char buffer[MAXLINE]; 
    struct sockaddr_in servaddr, cliaddr; 
    socklen_t clientlen = sizeof(cliaddr);


    SOCKTYPE socketfd = PrimaryClient::getInstance()->socketfd;
    //SOCKET socketfd = socket(AF_INET, SOCK_DGRAM, 0); 

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Socket: IPV4, Any connection, PORTNumber
    servaddr.sin_family    = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(ReceivingPort);
    

    int a = bind(socketfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    std::cout << " Bind Return: " << a << "\n";

    int error = WSAGetLastError();

    printf("bind failed with error: %d\n", error);

    if (a == -1) {
        #ifdef _WIN32
            closesocket(socketfd);
            WSACleanup();
        #endif
        return;
    }

    this->acceptIncoming = true;
    while (this->acceptIncoming)
    {
        incomingLoop(socketfd, buffer, cliaddr);
    }
    
    #ifdef _WIN32
        closesocket(socketfd);
        WSACleanup();
    #endif
}

void IncomingHandler::incomingLoop(SOCKTYPE socketfd, char* buffer, sockaddr_in cliaddr) {
    int clientlen;

    int packetlen = recvfrom(socketfd, buffer, MAXLINE,
        0, (struct sockaddr*)&cliaddr, &clientlen);


    if (packetlen < Packet::MIN_PACKET_SIZE) {
        return;
    }

    Packet incomingPacket(-1, PacketType::NONE, PrimaryClient::getInstance()->getClientID());
    int datalen = incomingPacket.deserialize(buffer);

    
    // Handle Diffrent Packet Types
    switch(incomingPacket.getPacketType()) {
        case PacketType::ACK:
            break;
        case PacketType::HANDSHAKE_REQUEST:
            this->handleConnectionRequest(&incomingPacket, socketfd, cliaddr, clientlen);
            break;
        case PacketType::HANDSHAKE_RESPONSE:
            this->handleConnectionResponse(&incomingPacket, socketfd, cliaddr, clientlen);
            break;
        case PacketType::RELAY_USER_INFO:
            handleRelayInfoResponse(&incomingPacket, datalen);
            break;
        case PacketType::PACKET:
            this->handlePacket(&incomingPacket, datalen, ntohs(cliaddr.sin_port));
            break;
        case PacketType::KEEP_ALIVE:
            //this->handleKeepAlive(&incomingPacket);
            break;
        default:
            std::cout << "Something is not right\n";
            exit(1);
    }
}

void IncomingHandler::handlePacket(Packet *incomingPacket, int datalen, int temp) { 
    //Remove the UserID here
    RemoteUser *packetAuthor = PrimaryClient::getInstance()->getUser(incomingPacket->packetAuthorID.getString());
    if (packetAuthor == NULL) {
        return;
    }



    // AAD Gen for the senderID and incoming length of the data
    unsigned char aad[UUID_BYTE_SIZE + sizeof(datalen)];
    memcpy(aad, incomingPacket->packetAuthorID.getRaw(), UUID_BYTE_SIZE);
    memcpy(aad+UUID_BYTE_SIZE, &datalen, sizeof(datalen));


    // Decrypt Here
    unsigned char output[datalen];
    if (symmetricDecryption((unsigned char*)incomingPacket->getData(), datalen, aad, sizeof(aad), incomingPacket->getTag(), 
            packetAuthor->connection.getSharedSecret(), incomingPacket->getIV(), AES_256_IV_LENGTH, output) < 1) {
        return;
    }


    // Get DataType
    DataTypes packetDataType;
    memcpy(&packetDataType, output, sizeof(DataTypes));

    switch (packetDataType) {
        case DataTypes::MESSAGETYPE:
            handleMessage(output);
        case DataTypes::FILETYPE:
            break;
    }
}

void IncomingHandler::handleMessage(unsigned char* decryptedData) {
    MessageContainer* msg = MessageContainer::deserialize(decryptedData);
    
    PrimaryClient* client = PrimaryClient::getInstance();
    //Get Server somehow
    Server* server = client->getServer(msg->getServerID()->getString());
    TextChannel* channel = server->knownChannels[msg->getChannelID()->getString()];

    channel->messages.push_back(msg);
    CppInterface::instancePtr->loadMessage(msg);
    
}


void IncomingHandler::handleConnectionRequest(Packet *packet, SOCKTYPE socketfd, sockaddr_in returnAdress, socklen_t returnLen) {
    PrimaryClient* client = PrimaryClient::getInstance();
    RemoteUser* userRequesting = client->getUser(packet->packetAuthorID.getString());
    if (userRequesting == NULL) {
        return;
    }
    
    unsigned char* hashOutput = new unsigned char[SHAW_256_HASH_SIZE];
    ML_KEM_Handshake::onRequest(packet, socketfd, returnAdress, returnLen, client->getClientID(), hashOutput);

    // set shared secret
    userRequesting->connection.setSharedSecret(hashOutput);
}

void IncomingHandler::handleConnectionResponse(Packet *packet, SOCKTYPE socketfd, sockaddr_in returnAdress, socklen_t returnLen) {
    PrimaryClient* client = PrimaryClient::getInstance();

    RemoteUser* userRequesting = client->getUser(packet->packetAuthorID.getString());
    if (userRequesting == NULL) {
        return;
    }

    unsigned char* hashOutput = new unsigned char[SHAW_256_HASH_SIZE];
    ML_KEM_Handshake::onReply(packet, client->getKeyPair(), userRequesting->connection.handshakeRandBuffer, hashOutput);
    
    // set shared secret
    userRequesting->connection.setSharedSecret(hashOutput);
    
}

void IncomingHandler::handleRelayInfoResponse(Packet* packet, int datalen) {
    sockaddr_in addr; 
    addr.sin_port;
    addr.sin_addr.s_addr;

    // Make sure that this packet has the right size requirements 
    if (datalen != (sizeof(addr.sin_addr.s_addr) + sizeof(addr.sin_port))) {
        return;
    }

    memcpy(&addr.sin_addr.s_addr, packet->getData(), sizeof(addr.sin_addr.s_addr));               // Copy Addr
    int offset = sizeof(addr.sin_addr.s_addr);
    memcpy(&addr.sin_port, packet->getData()+offset, sizeof(addr.sin_port));                 // Copy Port

    int port = ntohs(addr.sin_port);
    char *ip = inet_ntoa(addr.sin_addr);

    PrimaryClient* client = PrimaryClient::getInstance();

    RemoteUser* userRequesting = client->getUser(packet->packetAuthorID.getString());

    // if user doesn't exist create one
    if (userRequesting == nullptr) {
        if (!client->registerNewUser(&packet->packetAuthorID)) {
            return;
        }
        userRequesting = client->getUser(packet->packetAuthorID.getString());
    }
    
    userRequesting->connection.setAddr(ip, port);
    client->getOutgoingHandler()->enableConnection(userRequesting);
    return;
}

// If this user hasn't been registerd yet, register them ( May not be nessasry if they go through relay)
void IncomingHandler::handleKeepAlive(Packet* packet) {
    PrimaryClient* client = PrimaryClient::getInstance();
    RemoteUser* userRequesting = client->getUser(packet->packetAuthorID.getString())
    ;
    // if user doesn't exist create one
    if (userRequesting == nullptr) {
        if (!client->registerNewUser(&packet->packetAuthorID)) {
            return;
        }
        userRequesting = client->getUser(packet->packetAuthorID.getString());
    }
}