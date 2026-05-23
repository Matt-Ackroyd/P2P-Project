#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <cstring>
#include <stdio.h>
#include <iostream>

using namespace std;


int do_crypt(char *outfile) {
    /*
     * Bogus key and IV: we'd normally set these from
     * another source.
     */
    unsigned char key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,14};
    unsigned char iv[] = {1,2,3,4,5,6,7,8,1,2,3,4,5,6,7,8};
    const unsigned char plaintext[] = "This Isn't working...";
    int plaintext_len = sizeof(plaintext);
    EVP_CIPHER_CTX *ctx;
    FILE *out;

    int len;
    unsigned char ciphertext[1024];
    int ciphertext_len;
    unsigned char tag[16];


    ctx = EVP_CIPHER_CTX_new();
    if (!EVP_EncryptInit_ex2(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL)) {
        /* Error */
        printf("FUCKME \n");
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, 16, NULL);
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_SET_KEY_LENGTH, 32, NULL);

    // IV & Key
    EVP_EncryptInit_ex2(ctx, NULL, key, iv, NULL);

    // Encrypt
    EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len);
    ciphertext_len = len;
    
    /*
     * Buffer passed to EVP_EncryptFinal() must be after data just
     * encrypted to avoid overwriting it.
     */
    if (!EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) {
        /* Error */
        printf("FUCKME3 \n");;
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }
    ciphertext_len += len;

    
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag);
    for (const auto& e : tag) {
        std::cout << e;
    }
    cout << "\n";


    FILE *a;
    a = fopen("Tag", "wb");
    fwrite(tag, 1, 16, a);
    fclose(a);

    
    EVP_CIPHER_CTX_free(ctx);
    /*
     * Need binary mode for fopen because encrypted data is
     * binary data. Also cannot use strlen() on it because
     * it won't be NUL terminated and may contain embedded
     * NULs.
     */
    out = fopen(outfile, "wb");
    if (out == NULL) {
        /* Error */
        return 0;
    }
    fwrite(ciphertext, 1, ciphertext_len, out);
    fclose(out);
    return 1;
}

int undo_crypt(char *infile) {
    unsigned char outbuf[1024];
    unsigned char inbuf[1024];
    unsigned char tag[16];
    int outlen, tmplen;
    /*
     * Bogus key and IV: we'd normally set these from
     * another source.
     */
    unsigned char key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,14};
    unsigned char iv[] = {1,2,3,4,5,6,7,8,1,2,3,4,5,6,7,8};
    EVP_CIPHER_CTX *ctx;
    FILE *in;

    in = fopen(infile, "rb");
    if (in == NULL) {
        /* Error */
        cout << "FUCK";
        return 0;
    }
    fread(inbuf, sizeof(inbuf), 1, in);
    fclose(in);
    //cout << inbuf;
    in = fopen("Tag", "rb");
    if (in == NULL) {
        /* Error */
        cout << "FUCK";
        return 0;
    }
    fread(tag, sizeof(tag), 1, in);
    fclose(in);



    ctx = EVP_CIPHER_CTX_new();
    if (!EVP_DecryptInit_ex2(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL)) {
        /* Error */
        cout << "FUCK2";
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    // Set IV Length
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, 16, NULL);
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_SET_KEY_LENGTH, 32, NULL);
    
    // Set Key & IV
    EVP_DecryptInit_ex2(ctx, NULL, key, iv, NULL);

    // Decrypt
    if (!EVP_DecryptUpdate(ctx, outbuf, &outlen, inbuf, sizeof(inbuf))) {
        /* Error */
        cout << "FUCK3";
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    // set TAG
    for (const auto& e : tag) {
        std::cout << e;
    }
    cout << "\n";
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, tag);
    
   
    // Finish Decrypt
    cout << "Status: " << EVP_DecryptFinal_ex(ctx, outbuf + outlen, &tmplen) << "\n";
    
    EVP_CIPHER_CTX_free(ctx);

    
    cout << outbuf << "\n";
    return 1;
}

int maiasdasn() {
    cout << "Start: \n";
    do_crypt("CT.dat");
    //taghuh[4] = '0';
    //taghuh[1] = '5';
    undo_crypt("CT.dat");

}