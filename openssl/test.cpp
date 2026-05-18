#include <openssl/evp.h>
#include <openssl/rand.h>
#include <cstring>
#include <stdio.h>
#include <iostream>

using namespace std;


int do_crypt(char *outfile, unsigned char* tag) {
    unsigned char outbuf[1024];
    int outlen, tmplen;
    /*
     * Bogus key and IV: we'd normally set these from
     * another source.
     */
    unsigned char key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    unsigned char iv[] = {1,2,3,4,5,6,7,8,1,2,3,4,5,6,7,8};
    const unsigned char intext[] = "Some Crypto Text";
    EVP_CIPHER_CTX *ctx;
    FILE *out;

    ctx = EVP_CIPHER_CTX_new();
    if (!EVP_EncryptInit_ex2(ctx, EVP_aes_256_gcm(), key, iv, NULL)) {
        /* Error */
        printf("FUCKME \n");
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    if (!EVP_EncryptUpdate(ctx, outbuf, &outlen, intext, sizeof(intext))) {
        /* Error */
         printf("FUCKME2 \n");;
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }
    /*
     * Buffer passed to EVP_EncryptFinal() must be after data just
     * encrypted to avoid overwriting it.
     */
    if (!EVP_EncryptFinal_ex(ctx, outbuf + outlen, &tmplen)) {
        /* Error */
         printf("FUCKME3 \n");;
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    cout << EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag) << "\n";
    FILE *a;
    a = fopen("Tag", "wb");
    fwrite(tag, 1, 16, a);
    fclose(a);

    
    outlen += tmplen;
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
    fwrite(outbuf, 1, outlen, out);
    fclose(out);
    return 1;
}

int undo_crypt(char *infile, unsigned char* tag) {
    unsigned char outbuf[1024];
    unsigned char inbuf[1024];
    int outlen, tmplen;
    /*
     * Bogus key and IV: we'd normally set these from
     * another source.
     */
    unsigned char key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    unsigned char iv[] = {1,2,3,4,5,6,7,8,1,2,3,4,5,6,7,8};
    const unsigned char intext[] = "Some Crypto Text";
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



    ctx = EVP_CIPHER_CTX_new();
    if (!EVP_DecryptInit_ex2(ctx, EVP_aes_256_gcm(), key, iv, NULL)) {
        /* Error */
        cout << "FUCK2";
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    if (!EVP_DecryptUpdate(ctx, outbuf, &outlen, inbuf, sizeof(inbuf))) {
        /* Error */
        cout << "FUCK3";
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }
   

    EVP_DecryptFinal_ex(ctx, NULL, &tmplen);
    EVP_CIPHER_CTX_free(ctx);

    
    cout << outbuf << "\n";
    return 1;
}

int main() {
    unsigned char taghuh[16];
    cout << "STart: \n";
    do_crypt("CT.dat", taghuh);
    undo_crypt("CT.dat", taghuh);

}