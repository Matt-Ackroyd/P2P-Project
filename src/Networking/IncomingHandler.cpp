#include "IncomingHandler.h"
#include <unordered_set>

void IncomingHandler::acknowledgePacket(Packet packet, UDPConnection connectedUser) {

}


// Starts Up the Reciving Thread
void IncomingHandler::enableIncomingTraffic(int ReceivingPort) {
    std::thread t(&IncomingHandler::startReceiving, this, ReceivingPort);
    this->recvThread = move(t);
}

// Recive loop
void IncomingHandler::startReceiving(int ReceivingPort)
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

    printf("bind failed with error: %d\n", WSAGetLastError());

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
        recvfrom(socketfd, buffer, MAXLINE,
            0, (struct sockaddr*)&cliaddr, &clientlen);


        //cout << "sendback Port:" << ntohs(cliaddr.sin_port) << "\n";

        // TODO REPLACE THIS WITH A DEFINED PORT
        //cliaddr.sin_port = htons(5001);

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
            default:
                std::cout << "Something is not right\n";
                exit(1);
        }
    }
    
    #ifdef _WIN32
        closesocket(socketfd);
        WSACleanup();
    #endif
}

void IncomingHandler::handlePacket(Packet *incomingPacket, int datalen, int temp) { 
    //Remove the UserID here
    RemoteUser *packetAuthor = PrimaryClient::getInstance()->getUser(incomingPacket->packetAuthorID.getString());
    if (packetAuthor == NULL) {
        return;
    }

    // Temp for testing
    packetAuthor->connection->SendingPort = 5000;
    packetAuthor->connection->setAddr("127.0.0.1");


    

    //RemoteUser connectedUser(incomingPacket->packetAuthorID, NULL);
    //UDPConnection *connection = connectedUser.connection;
    // TODO Make sure this is per user
    //Ignore duplicates
    //if (this->nextExpectedSeqNum != incomingPacket->getSeqNum()) {
    //    return;
    //}

    // Acknowlage the packet 
    //this->acknowledgePacket(incomingPacket, connection);


    // AAD Gen for the senderID and incoming length of the data
    unsigned char aad[UUID_BYTE_SIZE + sizeof(datalen)];
    memcpy(aad, incomingPacket->packetAuthorID.getRaw(), UUID_BYTE_SIZE);
    memcpy(aad+UUID_BYTE_SIZE, &datalen, sizeof(datalen));


    // Decrypt Here
    unsigned char output[datalen];
    if (symmetricDecryption((unsigned char*)incomingPacket->getData(), datalen, aad, sizeof(aad), incomingPacket->getTag(), 
            packetAuthor->connection->getSharedSecret(), incomingPacket->getIV(), AES_256_IV_LENGTH, output) < 1) {
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
    unsigned char* hashOutput = new unsigned char[SHAW_256_HASH_SIZE];
    ML_KEM_Handshake::onRequest(packet, socketfd, returnAdress, returnLen, PrimaryClient::getInstance()->getClientID(), hashOutput);

    // User creation
    PrimaryClient::getInstance()->registerNewUser(packet->packetAuthorID, hashOutput);
}

void IncomingHandler::handleConnectionResponse(Packet *packet, SOCKTYPE socketfd, sockaddr_in returnAdress, socklen_t returnLen) {
    PrimaryClient* client = PrimaryClient::getInstance();

    unsigned char* hashOutput = new unsigned char[SHAW_256_HASH_SIZE];
    ML_KEM_Handshake::onReply(packet, client->getKeyPair(), client->handShakeRand, hashOutput);
    
    // User creation
    client->registerNewUser(packet->packetAuthorID, hashOutput);
    
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

    std::cout << "AAAAA\n";
}