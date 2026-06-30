#include "ML-KEM_Handshake.h"

void ML_KEM_Handshake::startHandshake(unsigned char* randomBuffer, EVP_PKEY* keyPair, SOCKTYPE socketfd, sockaddr_in connectionAddr, ID* yourID, int seqenceNumber) {
    Packet *packet = new Packet(seqenceNumber, PacketType::HANDSHAKE_REQUEST, yourID);
    unsigned char data[ML_KEM_HANDSHAKE_RANDSIZE + ML_KEM_KEYLENGTH];

    // Generate Random Number for handshake & store it for later in Primary Client
    RAND_bytes(randomBuffer, ML_KEM_HANDSHAKE_RANDSIZE);
    memcpy(data, randomBuffer, ML_KEM_HANDSHAKE_RANDSIZE);

    // Get Public key & place in data(offset by the randsize)
    size_t publen = ML_KEM_KEYLENGTH;
    EVP_PKEY_get_raw_public_key(keyPair, 
        data+ML_KEM_HANDSHAKE_RANDSIZE, &publen);
    
    int packetlen = packet->serialize((char*)data, ML_KEM_HANDSHAKE_RANDSIZE + ML_KEM_KEYLENGTH, NULL, NULL);
    sendto(socketfd, packet->getData(), packetlen, 0, (struct sockaddr*)&connectionAddr, sizeof(connectionAddr));
    delete packet;
}

int ML_KEM_Handshake::onRequest(Packet* packet, SOCKTYPE socketfd, sockaddr_in returnAdress, ID* yourID, unsigned char* outputedSecret, int seqenceNumber) {
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
        return 0;
    }

    if (!EVP_PKEY_encapsulate_init(ctx, NULL)) {
        return -1;
    }
    // ML KEM
    EVP_PKEY_encapsulate(ctx, NULL, &outlen, NULL, &secretlen);
    unsigned char out[ML_KEM_HANDSHAKE_RANDSIZE + ML_KEM_KEYLENGTH], secret[secretlen];
    if (EVP_PKEY_encapsulate(ctx, out+ML_KEM_HANDSHAKE_RANDSIZE, &outlen, secret, &secretlen) < 1) {
        return -2;
    }

    //Generate Random for yourself
    unsigned char selfRand[ML_KEM_HANDSHAKE_RANDSIZE];
    RAND_bytes(selfRand, ML_KEM_HANDSHAKE_RANDSIZE);
    // send the rand back
    memcpy(out, selfRand, ML_KEM_HANDSHAKE_RANDSIZE);

    // Create Return Packet
    Packet returnPacket(seqenceNumber, PacketType::HANDSHAKE_RESPONSE, yourID);
    int packetlen = returnPacket.serialize((char*)out, ML_KEM_HANDSHAKE_RANDSIZE + ML_KEM_KEYLENGTH, NULL, NULL);
    int aasd = sendto(socketfd, returnPacket.getData(), packetlen, 0, (const struct sockaddr *)&returnAdress, sizeof(returnAdress));
    int a = ntohs(returnAdress.sin_port);
    char *ip = inet_ntoa(returnAdress.sin_addr);

    
    // Hash the premaster with rand values + a salt
    handshakeHash(secret, secretlen, selfRand, rand, outputedSecret);
    return 1;
}



int ML_KEM_Handshake::onReply(Packet* packet, EVP_PKEY* KeyPair, unsigned char* selfRandom, unsigned char* outputedSecret) {
    EVP_PKEY_CTX *ctx = NULL;
    
    // Get pre-master & Random Values
    unsigned char rand[ML_KEM_HANDSHAKE_RANDSIZE];
    memcpy(&rand, packet->getData(), ML_KEM_HANDSHAKE_RANDSIZE);
    unsigned char out[ML_KEM_KEYLENGTH];
    memcpy(out, packet->getData()+ML_KEM_HANDSHAKE_RANDSIZE, ML_KEM_KEYLENGTH);

    ctx = EVP_PKEY_CTX_new_from_pkey(NULL, KeyPair, NULL);

    if (EVP_PKEY_decapsulate_init(ctx, NULL) < 1) {
        return 0;
    }

    size_t sLen;
    if (EVP_PKEY_decapsulate(ctx, NULL, &sLen, out, ML_KEM_KEYLENGTH) < 1) {
        return -1;
    }
    unsigned char sharedSecret[sLen];

    if (EVP_PKEY_decapsulate(ctx, sharedSecret, &sLen, out, ML_KEM_KEYLENGTH) < 1) {
        return -2;
    }

    // Hash the premaster with rand values + a salt
    handshakeHash(sharedSecret, sLen, rand, selfRandom, outputedSecret);
    return 1;
}