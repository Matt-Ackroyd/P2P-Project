#include "IncomingHandler.h"
#include "DatabaseConnection.h"
#include "FileHandler.h"


IncomingHandler::IncomingHandler(int ReceivingPort) {
    this->IncomingHandlerThread = std::thread(&IncomingHandler::incomingStartup, this, ReceivingPort);

    // for (int i = 0; i < threadCount; i++) {
    //     ThreadManager[i] = nullptr;
    // }
}


// Setup
void IncomingHandler::incomingStartup(int ReceivingPort)
{
    char buffer[MAXLINE]; 
    struct sockaddr_in servaddr; 

    SOCKTYPE socketfd = PrimaryClient::getInstance()->socketfd;
    //SOCKET socketfd = socket(AF_INET, SOCK_DGRAM, 0); 

    memset(&servaddr, 0, sizeof(servaddr));

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
            IncomingLoop(buffer);
        } catch (std::exception e) {
            // TODO Make more infomative 
            std::cout << "Incoming Exception: ";
            std::cout << e.what();
        }
    }
    
    #ifdef _WIN32
        closesocket(socketfd);
        WSACleanup();
    #endif
}

void IncomingHandler::IncomingLoop(char* buffer) {
    struct sockaddr_in cliaddr; 
    socklen_t clientlen = sizeof(cliaddr);
    memset(&cliaddr, 0, sizeof(cliaddr));

    int packetlen = recvfrom(PrimaryClient::getInstance()->socketfd, buffer, MAXLINE,
        0, (struct sockaddr*)&cliaddr, &clientlen);

    int error = WSAGetLastError();
    std::string ipa = inet_ntoa(cliaddr.sin_addr);
    int porta = ntohs(cliaddr.sin_port);

    // Thread Manager
    Packet* incomingPacket = new Packet(PacketType::NONE, PrimaryClient::getInstance()->getClientID());
    incomingPacket->deserialize(buffer);

    char* buffercpy = new char[MAXLINE];
    memcpy(buffercpy, buffer, MAXLINE);


    std::function<void()> newTask = std::bind(&IncomingHandler::threadStarter, this, buffercpy, cliaddr);

    ThreadManager.enqueue(newTask);

    // int i = 0;
    // bool jobAllocated = false;
    // while (!jobAllocated) {
    //     // Look for open thread
    //     if (ThreadManager[i] == nullptr) {
    //         threadMTX.lock();
    //         char* buffercpy = new char[MAXLINE];
    //         memcpy(buffercpy, buffer, MAXLINE);

    //         std::thread* newThread = new std::thread(&IncomingHandler::threadStarter, this, buffercpy, cliaddr, i);
    //         newThread->detach();
    //         ThreadManager[i] = newThread;
    //         jobAllocated = true;
    //         threadMTX.unlock();
    //     }
    //     i++;
    //     if (i == threadCount) {
    //         i = 0;
    //     }
    // }
}

void IncomingHandler::threadStarter(char* buffer, sockaddr_in cliaddr) {
    try { 
        onPacketRecived(buffer, cliaddr);
    }
    catch (std::exception e) {
        
    }
    // // Clean Up Thread
    // std::lock_guard<std::mutex> lock(threadMTX);
    // std::thread* threadPointer = this->ThreadManager[threadNumber];
    // this->ThreadManager[threadNumber] = nullptr;
    // delete threadPointer;
}

void IncomingHandler::onPacketRecived(char* buffer, sockaddr_in cliaddr) {
    Packet* incomingPacket = new Packet(PacketType::NONE, PrimaryClient::getInstance()->getClientID());
    incomingPacket->deserialize(buffer);
    delete buffer;

    // Relays dont have a userID so we need to handle the packet early 
    if (incomingPacket->getPacketType() == PacketType::RELAY_USER_INFO || incomingPacket->getPacketType() == RELAY_HANDSHAKE_RESPONSE) {
        handleIncoming(incomingPacket, cliaddr);
        return;
    }

    // Get The User who Sent it, if they dont exist then create them & Send any Buffered actions now that there exists a connection
    RemoteUser *packetAuthor = onIncomingPacket(incomingPacket, cliaddr);
    std::string packetID = incomingPacket->getPacketID();

    // If this packet doesn't need an acknowledgement just handle and dont worry about any queues
    if (incomingPacket->getPacketType() <= 3) {
        handleIncoming(incomingPacket, cliaddr);
        return;
    }


    // If the Packet is required to be acknowledged
    UDPConnection* userConnection = &packetAuthor->connection;

    // Duplicate Packets 
    if (userConnection->incommingBuffer.contains(packetID)) {
        userConnection->sendAck(packetID);
        return;
    }

    
    userConnection->addPacketToIncomingQueue(incomingPacket);  //Add the packet into the buffer

    handleIncoming(incomingPacket, cliaddr);

    userConnection->sendAck(packetID);
}



void IncomingHandler::handleIncoming(Packet* incomingPacket, sockaddr_in cliaddr) {
    // Handle Diffrent Packet Types
    switch(incomingPacket->getPacketType()) {
        case PacketType::KEEP_ALIVE:
            break;
        case PacketType::HELLO:
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
        case PacketType::RELAY_HANDSHAKE_RESPONSE:
            RelayClient::onRelayHandshakeResponse(incomingPacket, cliaddr);
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
    RemoteUser *packetAuthor = PrimaryClient::getInstance()->getUser(incomingPacket->packetAuthorID);
    if (packetAuthor == NULL) {
        return;
    }



    // AAD Gen for the senderID and incoming length of the data
    int datalen = incomingPacket->getDataLength();
    unsigned char aad[UUID_BYTE_SIZE + sizeof(datalen)];
    unsigned char uuid[UUID_BYTE_SIZE];
    ID::BytesFromString(incomingPacket->packetAuthorID, uuid);
    memcpy(aad, uuid, UUID_BYTE_SIZE);
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
            break;
        case DataTypes::FILETYPE:
            FileHandler::onFilePacketRecieved(output);
            break;
        case DataTypes::FILE_HOST_CLAIM:
            FileHostClaim::onReceived(output, packetAuthor);
            break;
        case DataTypes::FILE_INDICATOR:
            FileIndicator::onReceived(output, packetAuthor);
            break;
        case DataTypes::JOIN_REQUEST:
            handleJoinRequest(output, packetAuthor);
            break;
        case DataTypes::USER:
            UserContainer::onReceived(output);
            break;
        case DataTypes::SERVER:
            ServerContainer::onReceived(output);
            break;
        case DataTypes::TEXT_CHANNEL:
            handleNewTextChannel(output);
            break;
        case DataTypes::SYNC_REQUEST:
            SyncRequest::onSyncRequest(output, packetAuthor);
            break;
        case DataTypes::SYNC_RESPONSE:
            SyncResponse::onSyncResponse(output, packetAuthor);
            break;
        case DataTypes::ADD_OR_MODIFY_REQUEST:
            Request::onRequest(output, packetAuthor);
            break;
    }
}



void IncomingHandler::handleConnectionRequest(Packet *packet) {
    PrimaryClient* client = PrimaryClient::getInstance();
    RemoteUser* userRequesting = client->getUser(packet->packetAuthorID);
    if (userRequesting == NULL) {
        return;
    }
    
    unsigned char* hashOutput = new unsigned char[SHAW_256_HASH_SIZE];
    Packet* responsePacket = ML_KEM_Handshake::onRequest(packet, client->getClientID(), hashOutput, userRequesting->connection.newSeqNum(), client->getDSAkey(), &userRequesting->connection.DSAkey);
    userRequesting->connection.addPacketToOutgoingQueue(responsePacket);

    // set shared secret
    userRequesting->connection.setSharedSecret(hashOutput);
}

void IncomingHandler::handleConnectionResponse(Packet *packet) {
    PrimaryClient* client = PrimaryClient::getInstance();

    RemoteUser* userRequesting = client->getUser(packet->packetAuthorID);
    if (userRequesting == NULL) {
        return;
    }

    unsigned char* hashOutput = new unsigned char[SHAW_256_HASH_SIZE];
    ML_KEM_Handshake::onReply(packet, client->getKeyPair(), userRequesting->connection.handshakeRandBuffer, hashOutput, &userRequesting->connection.DSAkey);
    
    // set shared secret
    userRequesting->connection.setSharedSecret(hashOutput);

        // if there is a buffered invitation send it
    if (!userRequesting->connection.bufferedServerInvitation.empty()) {
        userRequesting->connection.sendJoinRequest(userRequesting->connection.bufferedServerInvitation);
        userRequesting->connection.bufferedServerInvitation = "";
    }

}

void IncomingHandler::handleRelayInfoResponse(Packet* packet) {
    sockaddr_in addr; 
    addr.sin_family = AF_INET;
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

    RemoteUser* userRequesting = client->getUser(packet->packetAuthorID);

    // if user doesn't exist create one
    if (userRequesting == nullptr) {
        if (!client->registerNewUser(packet->packetAuthorID)) {
            return;
        }
        userRequesting = client->getUser(packet->packetAuthorID);
    }
    
    userRequesting->connection.setAddr(ip, port);
    client->enableConnection(userRequesting);

    // Send a packet immediately to open up communications to the other side 
    userRequesting->connection.resetConnection();
    userRequesting->connection.sendHello();

    return;
}

// If this user hasn't been registerd yet, register them ( May not be nessasry if they go through relay)
void IncomingHandler::handleKeepAlive(Packet* packet) {
    PrimaryClient* client = PrimaryClient::getInstance();
    RemoteUser* userRequesting = client->getUser(packet->packetAuthorID);

    // if user doesn't exist create one
    if (userRequesting == NULL) {
        if (!client->registerNewUser(packet->packetAuthorID)) {
            return;
        }
        userRequesting = client->getUser(packet->packetAuthorID);
    }
}


void IncomingHandler::handleAck(Packet* packet) {
    PrimaryClient* client = PrimaryClient::getInstance();
    RemoteUser* userRequesting = client->getUser(packet->packetAuthorID);

    userRequesting->connection.receivedAck(packet->getPacketID());
}


void IncomingHandler::handleMessage(unsigned char* decryptedData) {
    MessageContainer* msg = MessageContainer::deserialize(decryptedData);

    if (!msg->verify()) {
        delete msg;
        return;
    }
    
    PrimaryClient* client = PrimaryClient::getInstance();

    Server* server = client->getServer(*msg->getServerID());
    TextChannel* channel = server->knownChannels[*msg->getChannelID()];

    channel->receiveMessage(msg);
    CppInterface::instancePtr->GUIloadMessage(msg);
    
}



void IncomingHandler::handleNewTextChannel(unsigned char* decryptedData) {
    // Add check that the user creating this channel has the permision to do so
    TextChannelContainer channel = TextChannelContainer::deserialize(decryptedData);

    PrimaryClient* client = PrimaryClient::getInstance();
    client->getServer(channel.getServerID())->createNewTextChannel(channel.getChannelID());
}



void IncomingHandler::handleJoinRequest(unsigned char* decryptedData, RemoteUser* requestee) {
    // Add check to make sure we have the permission to accept invitations

    JoinRequest request = JoinRequest::deserialize(decryptedData);

    std::string serverid = DatabaseConnection::getInvitationsServerFromDB(request.getInvitation());
    Server* server = PrimaryClient::getInstance()->getServer(serverid);

    // If there is no server asosiated with this invitation code bail
    if (server == nullptr) {
        return;
    }

    requestee->contactAddress = request.getContactAddress();
    requestee->contactPort = request.getContactPort();
    requestee->requiresRelay = request.getRelayRequired();

    // Send Other Users this persons connecton info
    for (auto [id, user]: server->knownUsers) {
        user->connection.sendAddUserToServerRequest(requestee, server);
    }

    // Send Server Info
    requestee->connection.sendServer(server);

    server->addNewUser(requestee);
}



RemoteUser* IncomingHandler::onIncomingPacket(Packet* incomingPacket, sockaddr_in cliaddr) {
    RemoteUser* packetAuthor = PrimaryClient::getInstance()->getUser(incomingPacket->packetAuthorID);
    if (packetAuthor == NULL) {
        if (!PrimaryClient::getInstance()->registerNewUser(incomingPacket->packetAuthorID)) {
            throw std::runtime_error("User Not Registered");
        }
        packetAuthor = PrimaryClient::getInstance()->getUser(incomingPacket->packetAuthorID);
        packetAuthor->connection.setAddr(inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
    }

    packetAuthor->connection.lastHeardFrom = std::chrono::system_clock::now();
    // Enable this connection if it wasn't already
    PrimaryClient::enableConnection(packetAuthor);

    // If there is any buffered actions then do them
    if (packetAuthor->connection.requestHandshakeOnceConnected) {
        packetAuthor->connection.sendHandshakeRequest();
        packetAuthor->connection.requestHandshakeOnceConnected = false;
    } else if (!packetAuthor->connection.bufferedServerInvitation.empty() && packetAuthor->connection.getSharedSecret() != nullptr) {
        // if there is a buffered invitation send it
        packetAuthor->connection.sendJoinRequest(packetAuthor->connection.bufferedServerInvitation);
        packetAuthor->connection.bufferedServerInvitation = "";
    }


    // Manage incoming buffer timeout
    packetAuthor->connection.mtx.lock();
    for (auto& [packetID, timestamp] : packetAuthor->connection.incommingBuffer) {
        if (timestamp + std::chrono::milliseconds(10000) < std::chrono::system_clock::now()) {
            packetAuthor->connection.removePacketFromIncomingQueue(packetID);
        }
    }
    packetAuthor->connection.mtx.unlock();

    return packetAuthor;
}