#include "IncomingHandler.h"

void IncomingHandler::acknowledgePacket(Packet packet, UDPConnection connectedUser) {
    packet.getSeqNum();
    // Create an ack packet and send it back
    Packet ack(this->nextExpectedSeqNum, PacketType::ACK);
    ack.serialize(NULL, 0, NULL, NULL);

    sendto(connectedUser.sock, ack.getData(), MAXLINE, 0, (struct sockaddr*)NULL, sizeof((struct sockaddr*)NULL));
}


// Starts Up the Reciving Thread
void IncomingHandler::enableIncomingTraffic(int ReceivingPort) {
    thread t(&IncomingHandler::startReceiving, this, ReceivingPort);
    this->recvThread = move(t);
}

// Recive loop
void IncomingHandler::startReceiving(int ReceivingPort)
{
    unsigned char buffer[MAXLINE]; 
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

    this->acceptIncoming = true;
    while (this->acceptIncoming)
    {
        recvfrom(socketfd, buffer, sizeof(buffer),
            0, (struct sockaddr*)&cliaddr, &clientlen);

        Packet *incomingPacket = new Packet(-1, PacketType::NONE);
        int datalen = incomingPacket->deserialize(buffer);

        // Remove the UserID here
        incomingPacket->senderID;
        RemoteUser connectedUser;


        // Handle Diffrent Packet Types
        switch(incomingPacket->getPacketType()) {
            case PacketType::ACK:
                break;
            case PacketType::CONNECTION_REQUEST:
                break;
            case PacketType::CONNECTION_RESPONSE:
                break;
            case PacketType::PACKET:
                this->handlePacket(connectedUser.connection, incomingPacket, datalen);
                break;
            default:
                cout << "Something is not right\n";
                exit(1);
        }
        delete incomingPacket;
    }
    
}

void IncomingHandler::handlePacket(UDPConnection connection, Packet *incomingPacket, int datalen) { 
    // TODO Make sure this is per user
    //Ignore duplicates
    //if (this->nextExpectedSeqNum != incomingPacket->getSeqNum()) {
    //    return;
    //}

    // Acknowlage the packet 
    //this->acknowledgePacket(incomingPacket, connection);

    // Revove the IV, Mac

    // AAD Gen for the senderID and incoming length of the data
    unsigned char aad[sizeof(incomingPacket->senderID) + sizeof(datalen)];
    memcpy(aad, &incomingPacket->senderID, sizeof(incomingPacket->senderID));
    memcpy(aad+sizeof(incomingPacket->senderID), &datalen, sizeof(datalen));

    // TEMP MUST REMOVE TODO
    unsigned char key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,14};
    connection.sharedSecret = key;


    // Decrypt Here
    unsigned char output[datalen];
    if (!symmetricDecryption(incomingPacket->getData(), datalen, aad, sizeof(aad), incomingPacket->getTag(), 
            connection.getSharedSecret(), incomingPacket->getIV(), AES_256_IV_LENGTH, output)) {
        exit(1);
    }

    cout << "Other: ";
    for (int i = 0; i < datalen; i++) {
        cout << output[i];
    }
    cout << "\n";

    // Get DataType
    DataTypes packetDataType;

    switch (packetDataType) {
        case DataTypes::MESSAGETYPE:
            break;
        case DataTypes::FILETYPE:
            break;
    }
    

}


void IncomingHandler::handleConnectionRequest(Packet packet) {
    EVP_PKEY_CTX *ctx = NULL;
    EVP_PKEY *pkey;
    size_t secretlen = 0, outlen = 0;

    // Get Rand & Public Key
    unsigned int rand;
    memcpy(&rand, packet.getData(), sizeof(unsigned int));
    unsigned char rawKey[KEYLENGTH];
    memcpy(rawKey, packet.getData()+sizeof(unsigned int), KEYLENGTH);



    pkey = EVP_PKEY_new_raw_public_key(NULL, NULL, rawKey, KEYLENGTH);

    ctx = EVP_PKEY_CTX_new_from_pkey(NULL, pkey, NULL);
    if (ctx == NULL) {
        return;
    }

    if (!EVP_PKEY_encapsulate_init(ctx, NULL)) {
        return;
    }

    EVP_PKEY_encapsulate(ctx, NULL, &outlen, NULL, &secretlen);
    unsigned char out[outlen], secret[secretlen];
    if (EVP_PKEY_encapsulate(ctx, out, &outlen, secret, &secretlen) < 1) {
        return;
    }

    // Create an ID???? for them and a user structure to remember them
    // Create a reponse packet and send it back
    PrimaryClient* client = PrimaryClient::getInstance();
    
    // Add Temp User
    RemoteUser tmpUser;
    
}

void IncomingHandler::handleConnectionResponse(Packet packet) {
    EVP_PKEY_CTX *ctx = NULL;
    PrimaryClient* client = PrimaryClient::getInstance();
    

    // Get pre-master & Random Values
    unsigned int rand;
    memcpy(&rand, packet.getData(), sizeof(unsigned int));
    unsigned char* out;
    memcpy(out, packet.getData()+sizeof(unsigned int), KEYLENGTH);


    ctx = EVP_PKEY_CTX_new_from_pkey(NULL, client->getKeyPair(), NULL);

    cout << "Dencapsulate Innit: " << EVP_PKEY_decapsulate_init(ctx, NULL) << "\n";

    size_t sLen;
    cout << "Dencapsulate Size Check: " << EVP_PKEY_decapsulate(ctx, NULL, &sLen, out, KEYLENGTH) << "\n";
    unsigned char sharedSecret[sLen];
    cout << sLen << "\n";
    EVP_PKEY_decapsulate(ctx, sharedSecret, &sLen, out, KEYLENGTH);
}