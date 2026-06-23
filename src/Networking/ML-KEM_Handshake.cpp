#include "ML-KEM_Handshake.h"

void startHandshake() {

}

unsigned char* onML_KEM_HandshakeRequest(Packet* packet, SOCKTYPE socketfd, sockaddr_in *returnAdress, socklen_t returnLen) {
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
        return NULL;
    }

    if (!EVP_PKEY_encapsulate_init(ctx, NULL)) {
        return NULL;
    }
    // ML KEM
    EVP_PKEY_encapsulate(ctx, NULL, &outlen, NULL, &secretlen);
    unsigned char out[ML_KEM_HANDSHAKE_RANDSIZE + ML_KEM_KEYLENGTH], secret[secretlen];
    if (EVP_PKEY_encapsulate(ctx, out+ML_KEM_HANDSHAKE_RANDSIZE, &outlen, secret, &secretlen) < 1) {
        return NULL;
    }

    //Generate Random for yourself
    unsigned char selfRand[ML_KEM_HANDSHAKE_RANDSIZE];
    RAND_bytes(selfRand, ML_KEM_HANDSHAKE_RANDSIZE);
    // send the rand back
    memcpy(out, selfRand, ML_KEM_HANDSHAKE_RANDSIZE);

    // Create Return Packet
    Packet *returnPacket = new Packet(-1, PacketType::HANDSHAKE_RESPONSE);
    int packetlen = returnPacket->serialize((char*)out, ML_KEM_HANDSHAKE_RANDSIZE + ML_KEM_KEYLENGTH, NULL, NULL);
    sendto(socketfd, returnPacket->getData(), packetlen, 0, (const struct sockaddr *)returnAdress, returnLen);
    delete returnPacket;

    
    // Hash the premaster with rand values + a salt
    unsigned char* hashOutput = new unsigned char[32];
    handshakeHash(secret, secretlen, selfRand, rand, hashOutput);
    return hashOutput;
}



unsigned char* onML_KEM_HandshakeReply(Packet* packet, EVP_PKEY* KeyPair, unsigned char* random) {
    EVP_PKEY_CTX *ctx = NULL;
    //PrimaryClient* client = PrimaryClient::getInstance();
    
    // TODO ERROR CHECKING AND CLEANUP
    // Get pre-master & Random Values
    unsigned char rand[ML_KEM_HANDSHAKE_RANDSIZE];
    memcpy(&rand, packet->getData(), ML_KEM_HANDSHAKE_RANDSIZE);
    unsigned char out[ML_KEM_KEYLENGTH];
    memcpy(out, packet->getData()+ML_KEM_HANDSHAKE_RANDSIZE, ML_KEM_KEYLENGTH);

    ctx = EVP_PKEY_CTX_new_from_pkey(NULL, KeyPair, NULL);

    EVP_PKEY_decapsulate_init(ctx, NULL);

    size_t sLen;
    EVP_PKEY_decapsulate(ctx, NULL, &sLen, out, ML_KEM_KEYLENGTH);
    unsigned char sharedSecret[sLen];

    EVP_PKEY_decapsulate(ctx, sharedSecret, &sLen, out, ML_KEM_KEYLENGTH);



    // Hash the premaster with rand values + a salt
    unsigned char* hashOutput = new unsigned char[32];
    handshakeHash(sharedSecret, sLen, rand, random, hashOutput);
    return hashOutput;
}