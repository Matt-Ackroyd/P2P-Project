#include "IncomingHandler.h"
#include <unordered_set>

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
    cout << " Bind Return: " << a << "\n";

    this->acceptIncoming = true;
    while (this->acceptIncoming)
    {
        recvfrom(socketfd, buffer, MAXLINE,
            0, (struct sockaddr*)&cliaddr, &clientlen);

        // TODO REPLACE THIS WITH A DEFINED PORT
        cliaddr.sin_port = htons(5000);

        Packet *incomingPacket = new Packet(-1, PacketType::NONE);
        int datalen = incomingPacket->deserialize(buffer);


        // Handle Diffrent Packet Types
        switch(incomingPacket->getPacketType()) {
            case PacketType::ACK:
                break;
            case PacketType::CONNECTION_REQUEST:
                this->handleConnectionRequest(incomingPacket, socketfd, &cliaddr, clientlen);
                break;
            case PacketType::CONNECTION_RESPONSE:
                this->handleConnectionResponse(incomingPacket, socketfd, &cliaddr, clientlen);
                break;
            case PacketType::PACKET:
                this->handlePacket(incomingPacket, datalen);
                break;
            default:
                cout << "Something is not right\n";
                exit(1);
        }
        delete incomingPacket;
    }
    
}

void IncomingHandler::handlePacket(Packet *incomingPacket, int datalen) { 
    //Remove the UserID here
    RemoteUser *packetAuthor = PrimaryClient::getInstance()->getUser(incomingPacket->packetAuthorID.get());
    

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
    if (!symmetricDecryption(incomingPacket->getData(), datalen, aad, sizeof(aad), incomingPacket->getTag(), 
            packetAuthor->connection->getSharedSecret(), incomingPacket->getIV(), AES_256_IV_LENGTH, output)) {
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


void IncomingHandler::handleConnectionRequest(Packet *packet, int socketfd, sockaddr_in *returnAdress, socklen_t returnLen) {
    EVP_PKEY_CTX *ctx = NULL;
    EVP_PKEY *pkey;
    size_t secretlen = 0, outlen = 0;

    // Get Rand & Public Key
    unsigned char rand[ML_KEM_HANDSHAKE_RANDSIZE];
    memcpy(&rand, packet->getData(), ML_KEM_HANDSHAKE_RANDSIZE);
    unsigned char rawKey[ML_KEM_KEYLENGTH];
    memcpy(rawKey, packet->getData()+ML_KEM_HANDSHAKE_RANDSIZE, ML_KEM_KEYLENGTH);


    // ML KEM Innit
    pkey = EVP_PKEY_new_raw_public_key_ex(NULL, "ML-KEM-1024", NULL, rawKey, ML_KEM_KEYLENGTH);
    ctx = EVP_PKEY_CTX_new_from_pkey(NULL, pkey, NULL);
    if (ctx == NULL) {
        return;
    }

    if (!EVP_PKEY_encapsulate_init(ctx, NULL)) {
        return;
    }
    // ML KEM
    EVP_PKEY_encapsulate(ctx, NULL, &outlen, NULL, &secretlen);
    unsigned char out[ML_KEM_HANDSHAKE_RANDSIZE + ML_KEM_KEYLENGTH], secret[secretlen];
    if (EVP_PKEY_encapsulate(ctx, out+ML_KEM_HANDSHAKE_RANDSIZE, &outlen, secret, &secretlen) < 1) {
        return;
    }

    //Generate Random for yourself
    unsigned char selfRand[ML_KEM_HANDSHAKE_RANDSIZE];
    RAND_bytes(selfRand, ML_KEM_HANDSHAKE_RANDSIZE);
    // send the rand back
    memcpy(out, selfRand, ML_KEM_HANDSHAKE_RANDSIZE);

    // Create Return Packet
    Packet *returnPacket = new Packet(-1, PacketType::CONNECTION_RESPONSE);
    int packetlen = returnPacket->serialize(out, ML_KEM_HANDSHAKE_RANDSIZE + ML_KEM_KEYLENGTH, NULL, NULL);
    sendto(socketfd, returnPacket->getData(), packetlen, 0, (const struct sockaddr *)returnAdress, returnLen);
    delete returnPacket;

    
    // Hash the premaster with rand values + a salt
    unsigned char* hashOutput = new unsigned char[32];
    handshakeHash(secret, secretlen, selfRand, rand, hashOutput);
    
    // User creation
    PrimaryClient::getInstance()->registerNewUser(packet->packetAuthorID, hashOutput);
}

void IncomingHandler::handleConnectionResponse(Packet *packet, int socketfd, sockaddr_in *returnAdress, socklen_t returnLen) {
    EVP_PKEY_CTX *ctx = NULL;
    PrimaryClient* client = PrimaryClient::getInstance();
    
    // TODO ERROR CHECKING AND CLEANUP
    // Get pre-master & Random Values
    unsigned char rand[ML_KEM_HANDSHAKE_RANDSIZE];
    memcpy(&rand, packet->getData(), ML_KEM_HANDSHAKE_RANDSIZE);
    unsigned char out[ML_KEM_KEYLENGTH];
    memcpy(out, packet->getData()+ML_KEM_HANDSHAKE_RANDSIZE, ML_KEM_KEYLENGTH);

    ctx = EVP_PKEY_CTX_new_from_pkey(NULL, client->getKeyPair(), NULL);

    EVP_PKEY_decapsulate_init(ctx, NULL);

    size_t sLen;
    EVP_PKEY_decapsulate(ctx, NULL, &sLen, out, ML_KEM_KEYLENGTH);
    unsigned char sharedSecret[sLen];

    EVP_PKEY_decapsulate(ctx, sharedSecret, &sLen, out, ML_KEM_KEYLENGTH);



    // Hash the premaster with rand values + a salt
    unsigned char* hashOutput = new unsigned char[32];
    handshakeHash(sharedSecret, sLen, rand, client->handShakeRand, hashOutput);
    
    // User creation
    client->registerNewUser(packet->packetAuthorID, hashOutput);
    
}