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
    cout << " Bind Return: \n" << a;

    this->acceptIncoming = true;
    while (this->acceptIncoming)
    {
        recvfrom(socketfd, buffer, MAXLINE,
            0, (struct sockaddr*)&cliaddr, &clientlen);

        // TODO REPLACE THIS WITH A DEFINED PORT
        cliaddr.sin_port = htons(5000);

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
                this->handleConnectionRequest(incomingPacket, socketfd, &cliaddr, clientlen);
                break;
            case PacketType::CONNECTION_RESPONSE:
                this->handleConnectionResponse(incomingPacket, socketfd, &cliaddr, clientlen);
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


void IncomingHandler::handleConnectionRequest(Packet *packet, int socketfd, sockaddr_in *returnAdress, socklen_t returnLen) {
    EVP_PKEY_CTX *ctx = NULL;
    EVP_PKEY *pkey;
    size_t secretlen = 0, outlen = 0;

    // Get Rand & Public Key
    unsigned char rand[ML_KEM_HANDSHAKE_RANDSIZE];
    memcpy(&rand, packet->getData(), ML_KEM_HANDSHAKE_RANDSIZE);
    unsigned char rawKey[ML_KEM_KEYLENGTH];
    memcpy(rawKey, packet->getData()+ML_KEM_HANDSHAKE_RANDSIZE, ML_KEM_KEYLENGTH);



    pkey = EVP_PKEY_new_raw_public_key_ex(NULL, "ML-KEM-1024", NULL, rawKey, ML_KEM_KEYLENGTH);

    ctx = EVP_PKEY_CTX_new_from_pkey(NULL, pkey, NULL);
    if (ctx == NULL) {
        return;
    }

    if (!EVP_PKEY_encapsulate_init(ctx, NULL)) {
        return;
    }

    EVP_PKEY_encapsulate(ctx, NULL, &outlen, NULL, &secretlen);
    unsigned char out[ML_KEM_HANDSHAKE_RANDSIZE + ML_KEM_KEYLENGTH], secret[secretlen];

    if (EVP_PKEY_encapsulate(ctx, out+ML_KEM_HANDSHAKE_RANDSIZE, &outlen, secret, &secretlen) < 1) {
        return;
    }

    FILE *a;
    a = fopen("KEM-ML_OUT1.bin", "wb");
    fwrite(out+ML_KEM_HANDSHAKE_RANDSIZE, 1, outlen, a);
    fclose(a);

    // TODO CLEAN
    //Generate Random this yourself
    RAND_bytes(out, ML_KEM_HANDSHAKE_RANDSIZE);

    // Create Return Packet
    Packet *returnPacket = new Packet(-1, PacketType::CONNECTION_RESPONSE);
    int packetlen = returnPacket->serialize(out, ML_KEM_HANDSHAKE_RANDSIZE + ML_KEM_KEYLENGTH, NULL, NULL);
    sendto(socketfd, returnPacket->getData(), packetlen, 0, (const struct sockaddr *)returnAdress, returnLen);
    delete returnPacket;

    // Create an ID???? for them and a user structure to remember them
    // Create a reponse packet and send it back
    PrimaryClient* client = PrimaryClient::getInstance();
    
    // Add Temp User
    RemoteUser tmpUser;

    // Hash
    string saltStr = "Handshake";
    int hashInputlen = secretlen + 2*ML_KEM_HANDSHAKE_RANDSIZE + saltStr.length()+1;
    unsigned char hashInput[hashInputlen];

    memcpy(hashInput, secret, secretlen);
    int offset = secretlen;

    memcpy(hashInput+offset, rand, ML_KEM_HANDSHAKE_RANDSIZE);
    offset += ML_KEM_HANDSHAKE_RANDSIZE;

    memcpy(hashInput+offset, secret, ML_KEM_HANDSHAKE_RANDSIZE);
    offset += ML_KEM_HANDSHAKE_RANDSIZE;

    memcpy(hashInput+offset, (unsigned char*)saltStr.c_str(), saltStr.length()+1);

    unsigned char hashOutput[32];
    shaw256Hash(hashInput, hashInputlen, hashOutput);
    //cout << (uint32_t)hashOutput;
    //unordered_set<unsigned char[32]> test;
    
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
}