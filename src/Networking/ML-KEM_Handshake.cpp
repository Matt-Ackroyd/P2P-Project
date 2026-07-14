#include "ML-KEM_Handshake.h"

Packet* ML_KEM_Handshake::startHandshake(unsigned char* randomBuffer, EVP_PKEY* keyPair, std::string* yourID, int seqenceNumber, EVP_PKEY* yourDSAkey) {
    Packet *packet = new Packet(seqenceNumber, PacketType::HANDSHAKE_REQUEST, yourID);
    int size = ML_KEM_HANDSHAKE_RANDSIZE + ML_KEM_KEYLENGTH + ML_DSA_87_PUBLIC_KEY_BYTE_SIZE;
    unsigned char data[size];
    int offset = 0;

    // Generate Random Number for handshake & store it for later in Primary Client
    RAND_bytes(randomBuffer, ML_KEM_HANDSHAKE_RANDSIZE);
    memcpy(data, randomBuffer, ML_KEM_HANDSHAKE_RANDSIZE);
    offset += ML_KEM_HANDSHAKE_RANDSIZE;

    // Get Public key & place in data(offset by the randsize)
    size_t publen = ML_KEM_KEYLENGTH;
    EVP_PKEY_get_raw_public_key(keyPair, 
        data+offset, &publen);
    offset += ML_KEM_KEYLENGTH;

    // DSA-PUBLIC KEY
    if (yourDSAkey != NULL) {
        size_t DSAlen = ML_DSA_87_PUBLIC_KEY_BYTE_SIZE;
        EVP_PKEY_get_raw_public_key(yourDSAkey, NULL, &DSAlen);
        EVP_PKEY_get_raw_public_key(yourDSAkey, data+offset, &DSAlen);
    }
    
    packet->serialize((char*)data, size, NULL, NULL);
    return packet;
}

Packet* ML_KEM_Handshake::onRequest(Packet* packet, std::string* yourID, unsigned char* outputedSecret, int seqenceNumber, EVP_PKEY* yourDSAkey, EVP_PKEY* theirDSAkey, PacketType type) {
    EVP_PKEY_CTX *ctx = NULL;
    EVP_PKEY *pkey;
    size_t secretlen = 0, outlen = 0;

    // Get Rand & Public Key
    unsigned char rand[ML_KEM_HANDSHAKE_RANDSIZE];
    memcpy(&rand, packet->getData(), ML_KEM_HANDSHAKE_RANDSIZE);
    int offset = ML_KEM_HANDSHAKE_RANDSIZE;

    unsigned char rawKey[ML_KEM_KEYLENGTH];
    memcpy(rawKey, packet->getData()+offset, ML_KEM_KEYLENGTH);
    offset += ML_KEM_KEYLENGTH;

    unsigned char rawDSAkey[ML_DSA_87_PUBLIC_KEY_BYTE_SIZE];
    // Do not copy DSA key if this is a relay handshake response
    if (type != PacketType::RELAY_HANDSHAKE_RESPONSE) {
        memcpy(rawDSAkey, packet->getData()+offset, ML_KEM_KEYLENGTH);
    }


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
    unsigned char out[ML_KEM_KEYLENGTH];
    unsigned char secret[secretlen];
    if (EVP_PKEY_encapsulate(ctx, out, &outlen, secret, &secretlen) < 1) {
        return NULL;
    }

    //Generate Random for yourself
    unsigned char selfRand[ML_KEM_HANDSHAKE_RANDSIZE];
    RAND_bytes(selfRand, ML_KEM_HANDSHAKE_RANDSIZE);

    // Hash the premaster with rand values + a salt
    handshakeHash(secret, secretlen, selfRand, rand, outputedSecret);

    if (type != PacketType::RELAY_HANDSHAKE_RESPONSE) {
        theirDSAkey = EVP_PKEY_new_raw_public_key_ex(NULL, "ML-DSA-87", NULL, rawDSAkey, ML_DSA_87_PUBLIC_KEY_BYTE_SIZE);
    }

    // Return packet setup
    int returnLength = ML_KEM_HANDSHAKE_RANDSIZE + ML_KEM_KEYLENGTH + ML_DSA_87_PUBLIC_KEY_BYTE_SIZE;
    unsigned char returnBuffer[returnLength];
    // send the rand back
    memcpy(returnBuffer, selfRand, ML_KEM_HANDSHAKE_RANDSIZE);
    offset = ML_KEM_HANDSHAKE_RANDSIZE;

    memcpy(returnBuffer, out, ML_KEM_KEYLENGTH);
    offset = ML_KEM_KEYLENGTH;

    // DSA-PUBLIC KEY
    if (type != PacketType::RELAY_HANDSHAKE_RESPONSE) {
        size_t DSAlen = ML_DSA_87_PUBLIC_KEY_BYTE_SIZE;
        EVP_PKEY_get_raw_public_key(yourDSAkey, NULL, &DSAlen);
        EVP_PKEY_get_raw_public_key(yourDSAkey, returnBuffer+offset, &DSAlen);
    }

    // Create Return Packet
    Packet* returnPacket = new Packet(seqenceNumber, type, yourID);
    returnPacket->serialize((char*)out, returnLength, NULL, NULL);

    return returnPacket;
}



int ML_KEM_Handshake::onReply(Packet* packet, EVP_PKEY* KeyPair, unsigned char* selfRandom, unsigned char* outputedSecret, EVP_PKEY* theirDSAkey) {
    EVP_PKEY_CTX *ctx = NULL;
    
    // Get pre-master & Random Values
    unsigned char rand[ML_KEM_HANDSHAKE_RANDSIZE];
    memcpy(&rand, packet->getData(), ML_KEM_HANDSHAKE_RANDSIZE);
    int offset = ML_KEM_HANDSHAKE_RANDSIZE;

    unsigned char out[ML_KEM_KEYLENGTH];
    memcpy(out, packet->getData()+offset, ML_KEM_KEYLENGTH);
    offset += ML_KEM_KEYLENGTH;

    unsigned char rawDSAkey[ML_DSA_87_PUBLIC_KEY_BYTE_SIZE];
    // If this came from a relay they DID not send a DSA key so dont try copying it
    if (packet->getPacketType() != PacketType::RELAY_HANDSHAKE_RESPONSE) {
        memcpy(rawDSAkey, packet->getData()+offset, ML_KEM_KEYLENGTH);
    }

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

    if (packet->getPacketType() != PacketType::RELAY_HANDSHAKE_RESPONSE) {
        theirDSAkey = EVP_PKEY_new_raw_public_key_ex(NULL, "ML-DSA-87", NULL, rawDSAkey, ML_DSA_87_PUBLIC_KEY_BYTE_SIZE);
    }
    return 1;
}