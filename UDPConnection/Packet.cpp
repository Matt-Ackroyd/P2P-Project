#include "Packet.h"


void Packet::innit(char* Data, long unsigned int DataLength, PacketType packetType) {
    this->packetType = packetType;
    this->data = Data;
    this->dataLength = DataLength;
}

char* Packet::encaplulate() {
    this->packetLength = sizeof(this->seqNum) + sizeof(this->dataLength) + sizeof(this->packetType) + this->dataLength;
    this->encaplulatedPacket = new char[this->packetLength];    

    // TODO add safty checks
    // Add SeqNum to Output 
    memcpy(this->encaplulatedPacket, &this->seqNum, sizeof(this->seqNum));

    // Add Data Length to Output
    long unsigned int offset = sizeof(this->seqNum);
    memcpy(this->encaplulatedPacket+offset, &this->dataLength, sizeof(this->dataLength));

    // Add Data Type to Output
    offset += sizeof(this->dataLength);
    memcpy(this->encaplulatedPacket+offset, &this->packetType, sizeof(this->packetType));

    // Add Data to Output
    offset += sizeof(this->packetType);
    memcpy(this->encaplulatedPacket+offset, this->data, this->dataLength);

    return this->encaplulatedPacket;
}

void Packet::dencapsulate(char* recivedPacket) {
    //TODO Safty Checks
    long unsigned int offset = 0;
    memcpy(&this->seqNum, recivedPacket+offset, sizeof(this->seqNum));

    offset += sizeof(this->seqNum);
    memcpy(&this->dataLength, recivedPacket+offset, sizeof(this->dataLength));

    offset += sizeof(this->dataLength);
    memcpy(&this->packetType, recivedPacket+offset, sizeof(this->packetType));

    offset += sizeof(this->packetType);
    this->data = new char[dataLength];
    memcpy(this->data, recivedPacket+offset, this->dataLength);

    

}

int Packet::getSeqNum() {
    return this->seqNum;
}
void Packet::setSeqNum(int SeqNum) {
    this->seqNum = SeqNum;
}
long unsigned int Packet::getDataLength() {
    return this->dataLength;
}
PacketType Packet::getPacketType() {
    return this->packetType;
}
char* Packet::getData() {
    return this->data;
}
long unsigned int Packet::getPacketLength() {
    return this->packetLength;
}


void Packet::cleanupAfterSend() {
    delete this->encaplulatedPacket;
}
void Packet::cleanupAfterReceive() {
    delete this->data;
}





void Packet::encrypt(const unsigned char* plaintext, const unsigned char* key) {
    unsigned char iv[AES_256_IVEC_LENGTH];

    if (!RAND_bytes(iv, AES_256_IVEC_LENGTH))
        cout << "Rand Error";
        exit(1);

    unsigned char outbuf[1024];
    int outlen, tmplen;
    /*
     * Bogus key and IV: we'd normally set these from
     * another source.
     */
    char intext[] = "Some Crypto Text";
    EVP_CIPHER_CTX *ctx;
    FILE *out;

    if (!EVP_EncryptInit_ex2(ctx, EVP_aes_256_gcm(), key, iv, NULL)) {
        /* Error */
        EVP_CIPHER_CTX_free(ctx);
        return;
    }
    if (!EVP_EncryptUpdate(ctx, outbuf, &outlen, plaintext, sizeof(plaintext))) {
        /* Error */
        EVP_CIPHER_CTX_free(ctx);
        return;
    }
    /*
     * Buffer passed to EVP_EncryptFinal() must be after data just
     * encrypted to avoid overwriting it.
     */
    if (!EVP_EncryptFinal_ex(ctx, outbuf + outlen, &tmplen)) {
        /* Error */
        EVP_CIPHER_CTX_free(ctx);
        return;
    }
    outlen += tmplen;
    EVP_CIPHER_CTX_free(ctx);


}
