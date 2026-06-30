#include "IncomingHandler.h"

IncomingHandler::IncomingHandler(int ReceivingPort) {
    this->IncomingHandlerThread = std::thread(&IncomingHandler::incomingStartup, this, ReceivingPort);
}


// Setup
void IncomingHandler::incomingStartup(int ReceivingPort)
{
    char buffer[MAXLINE]; 
    struct sockaddr_in servaddr, cliaddr; 

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
    while (this->acceptIncoming) {
        try {
            IncomingLoop(buffer, cliaddr);
        } catch (...) {
            // TODO Make more infomative 
            std::cout << "Incoming Exception";
        }
    }
    
    #ifdef _WIN32
        closesocket(socketfd);
        WSACleanup();
    #endif
}

void IncomingHandler::IncomingLoop(char* buffer, sockaddr_in clifdsaddr) {
    struct sockaddr_in cliaddr; 
    socklen_t clientlen = sizeof(cliaddr);
    memset(&cliaddr, 0, sizeof(cliaddr));

    int packetlen = recvfrom(PrimaryClient::getInstance()->socketfd, buffer, MAXLINE,
        0, (struct sockaddr*)&cliaddr, &clientlen);

    int error = WSAGetLastError();

    if (packetlen < Packet::MIN_PACKET_SIZE) {
        throw std::runtime_error("Invalid Packet");
    }

    Packet* incomingPacket = new Packet(-1, PacketType::NONE, PrimaryClient::getInstance()->getClientID());
    incomingPacket->deserialize(buffer);
    incomingPacket->returnAddr = cliaddr;

    // If this packet doesn't need an acknowledgement just handle and dont worry about any queues
    if (incomingPacket->getPacketType() <= 3) {
        handleIncoming(incomingPacket);
        return;
    }


    // If the Packet is required to be acknowledged
    RemoteUser *packetAuthor = PrimaryClient::getInstance()->getUser(incomingPacket->packetAuthorID.getString());
    if (packetAuthor == NULL) {
        throw std::runtime_error("User Not Registered");
    }

    UDPConnection* userConnection = &packetAuthor->connection;
    // Duplicate Packets 
    if (userConnection->incomingSeqNum > incomingPacket->getSeqNum()) {
        userConnection->sendAck(userConnection->incomingSeqNum);
        return;
    }

    
    userConnection->addPacketToIncomingQueue(incomingPacket);  //Add the packet in sorted order

    // Keep handling packets as long as there as some and we have the next expected packet
    while (!userConnection->incommingBuffer.empty()) {
        Packet* front = userConnection->incommingBuffer.front();
        if (front->getSeqNum() != userConnection->incomingSeqNum) {
            break;
        }
        userConnection->incommingBuffer.pop_front();
        this->handleIncoming(front);
        userConnection->incomingSeqNum++;
        
    }
}


void IncomingHandler::handleIncoming(Packet* incomingPacket) {
    // Handle Diffrent Packet Types
    switch(incomingPacket->getPacketType()) {
        case PacketType::KEEP_ALIVE:
            this->handleKeepAlive(incomingPacket);
            break;
        case PacketType::ACK:
            this->handleAck(incomingPacket);
            break;
        case PacketType::RELAY_USER_INFO:
            handleRelayInfoResponse(incomingPacket);
            break;
        // The Following Types of Packets Require acks 
        case PacketType::HANDSHAKE_REQUEST:
            this->handleConnectionRequest(incomingPacket);
            break;
        case PacketType::HANDSHAKE_RESPONSE:
            this->handleConnectionResponse(incomingPacket);
            break;
        case PacketType::PACKET:
            this->handlePacket(incomingPacket);
            break;
        default:
            std::cout << "Something is not right\n";
            exit(1);
    }
    delete incomingPacket;
}

void IncomingHandler::handlePacket(Packet *incomingPacket) { 
    //Remove the UserID here
    RemoteUser *packetAuthor = PrimaryClient::getInstance()->getUser(incomingPacket->packetAuthorID.getString());
    if (packetAuthor == NULL) {
        return;
    }



    // AAD Gen for the senderID and incoming length of the data
    int datalen = incomingPacket->getDataLength();
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


void IncomingHandler::handleConnectionRequest(Packet *packet) {
    PrimaryClient* client = PrimaryClient::getInstance();
    RemoteUser* userRequesting = client->getUser(packet->packetAuthorID.getString());
    if (userRequesting == NULL) {
        return;
    }
    
    unsigned char* hashOutput = new unsigned char[SHAW_256_HASH_SIZE];
    ML_KEM_Handshake::onRequest(packet, client->socketfd, packet->returnAddr, client->getClientID(), hashOutput, userRequesting->connection.newSeqNum());

    // set shared secret
    userRequesting->connection.setSharedSecret(hashOutput);
}

void IncomingHandler::handleConnectionResponse(Packet *packet) {
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

void IncomingHandler::handleRelayInfoResponse(Packet* packet) {
    sockaddr_in addr; 
    addr.sin_port;
    addr.sin_addr.s_addr;

    // Make sure that this packet has the right size requirements 
    if (packet->getDataLength() != (sizeof(addr.sin_addr.s_addr) + sizeof(addr.sin_port))) {
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
    RemoteUser* userRequesting = client->getUser(packet->packetAuthorID.getString());

    // if user doesn't exist create one
    if (userRequesting == NULL) {
        if (!client->registerNewUser(&packet->packetAuthorID)) {
            return;
        }
        userRequesting = client->getUser(packet->packetAuthorID.getString());
    }
}


void IncomingHandler::handleAck(Packet* packet) {
    PrimaryClient* client = PrimaryClient::getInstance();
    RemoteUser* userRequesting = client->getUser(packet->packetAuthorID.getString());
    
    // if user doesn't abort
    if (userRequesting == nullptr) {
        if (!client->registerNewUser(&packet->packetAuthorID)) {
            return;
        }
    }

    userRequesting->connection.receivedAck(packet->getSeqNum());
}