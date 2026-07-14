#pragma once
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/pem.h>
#include <iostream>
#include <cstring>

//#define AES_256_KEY_LENGTH      32
//#define AES_256_KEY_LENGTH_BITS 256
#define AES_256_IV_LENGTH     32
#define AES_256_GCM_TAG_LENGTH  16

#define SHAW_256_HASH_SIZE 32

// Size of the 
#define ML_KEM_KEYLENGTH 1568
#define ML_KEM_HANDSHAKE_RANDSIZE 32

#define ML_DSA_87_PUBLIC_KEY_BYTE_SIZE 2592 
#define ML_DSA_87_PRIVATE_KEY_BYTE_SIZE 4896 
#define ML_DSA_87_SIGNATURE_BYTE_SIZE 4627

int symmetricEncryption(unsigned char *plaintext, int plaintext_len,
                unsigned char *aad, int aad_len,
                unsigned char *key,
                unsigned char *iv, int iv_len,
                unsigned char *ciphertext,
                unsigned char *tag);
int symmetricDecryption(unsigned char *ciphertext, int ciphertext_len,
                unsigned char *aad, int aad_len,
                unsigned char *tag,
                unsigned char *key,
                unsigned char *iv, int iv_len,
                unsigned char *plaintext);

int shaw256Hash(unsigned char* input, int inputlen, unsigned char* shaw256output);

void handshakeHash(unsigned char* premaster, int premasterlen, unsigned char* rand1, unsigned char* rand2, unsigned char* output);

void handleErrors();


EVP_PKEY* getDSAkeyFromFile();
void signMessage(EVP_PKEY *key, unsigned char *msg, size_t msg_len, unsigned char* signatureBuffer);
bool verifyMessage(EVP_PKEY *key, unsigned char* sig, unsigned char *msg, size_t msg_len);