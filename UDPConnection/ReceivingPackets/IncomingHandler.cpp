#include "IncomingHandler.h"

void IncomingHandler::acknowledgePacket(Packet packet, UDPConnection connectedUser) {
    packet.getSeqNum();
    // Create an ack packet and send it back
    Packet ack;
    ack.innit(nullptr, 0, PacketType::ACK);
    ack.setSeqNum(this->nextExpectedSeqNum);
    sendto(connectedUser.sock, ack.encaplulate(), MAXLINE, 0, (struct sockaddr*)NULL, sizeof((struct sockaddr*)NULL));
}


// Starts Up the Reciving Thread
void IncomingHandler::enableIncomingTraffic(int ReceivingPort) {
    thread t(&IncomingHandler::startReceiving, this, ReceivingPort);
    this->recvThread = move(t);
}

// Recive loop
void IncomingHandler::startReceiving(int ReceivingPort)
{
    char buffer[MAXLINE]; 
    struct sockaddr_in servaddr, cliaddr; 
    socklen_t clientlen = sizeof(cliaddr);

    int socketfd = socket(AF_INET, SOCK_DGRAM, 0);  

    bzero(&servaddr, sizeof(servaddr));
    bzero(&cliaddr, sizeof(cliaddr));

    // Socket: IPV4, Any connection, PORTNumber
    servaddr.sin_family    = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(ReceivingPort);

    int a = bind(socketfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    cout << " Bind Return: \n" << a;

    while (this->acceptIncoming)
    {
        recvfrom(socketfd, buffer, sizeof(buffer),
            0, (struct sockaddr*)&cliaddr, &clientlen);

        // Revove the IV, Mac and UserID here

        UDPConnection filler;

        // Decrypt Here

        Packet incomingPacket;
        incomingPacket.dencapsulate(buffer);

        // Packet Handling
        this->handlePacket(incomingPacket, filler);
    

        incomingPacket.cleanupAfterReceive();
    }
    
}

void IncomingHandler::handlePacket(Packet incomingPacket, UDPConnection connectedUser) {
    // Find assosiated user to this packet
    // If the incoming packet is an ACK then remove all packets before the acked one from the sending buffer
    if (incomingPacket.getPacketType() == PacketType::ACK) {
        
        for (int i = 0; i < connectedUser.sendingBuffer.size(); i++) {
            // Remove any packet that was acknowlaged by the cumulitive ack
            if (connectedUser.sendingBuffer.front().getSeqNum() < incomingPacket.getSeqNum()) {
                connectedUser.sendingBuffer.pop_front();
            }
        }
        // Early return because we dont need to do anything else with an ack packet
        return;
    }

    // If this packet is the one we are expecting update SeqNum
    if (this->nextExpectedSeqNum == incomingPacket.getSeqNum()) {
        this->nextExpectedSeqNum+1;
    }

    // Acknowlage the packet 
    this->acknowledgePacket(incomingPacket, connectedUser);

    //Ignore duplicates
    if (this->nextExpectedSeqNum != incomingPacket.getSeqNum()) {
        return;
    }

    
    // Handle Diffrent Packet Types
    switch(incomingPacket.getPacketType()) {
        case PacketType::ACK:
            break;
        case PacketType::CONNECTION_REQUEST:
            break;
        case PacketType::CONNECTION_RESPONSE:
            break;
        case PacketType::PACKET:
            break;
        default:
            cout << "Something is not right\n";
            exit(1);
        
    }




    if (incomingPacket.getDataType() == DataType::LARGEFILE) {
        // If the incoming file is larger than the maxsize
        int size = incomingPacket.getDataLength();
        if (incomingPacket.getDataLength() > MAXLINE) {
            size = MAXLINE;
        }

        ofstream myfile ("Test.png", ios::out | ios::app | ios::binary);
        myfile.write(incomingPacket.getData(), size);
    }
}