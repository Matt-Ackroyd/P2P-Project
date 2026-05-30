#pragma once
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <iostream>
using namespace std;

//#define AES_256_KEY_LENGTH      32
//#define AES_256_KEY_LENGTH_BITS 256
#define AES_256_IV_LENGTH     16
#define AES_256_GCM_TAG_LENGTH  16

// Size of the 
#define ML_KEM_KEYLENGTH 1568
#define ML_KEM_HANDSHAKE_RANDSIZE 32


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

void handleErrors();