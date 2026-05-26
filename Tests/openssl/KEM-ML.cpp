#include <openssl/evp.h>
#include <openssl/pem.h>
#include <iostream>

using namespace std;




int main() {
    EVP_PKEY_CTX *ctx = NULL;
    size_t secretlen = 0, outlen = 0;
    

    
    EVP_PKEY *pkey = EVP_PKEY_Q_keygen(NULL, NULL, "ML-KEM-1024");
    
    size_t publen;

    // Public key

    ctx = EVP_PKEY_CTX_new_from_pkey(NULL, pkey, NULL);
    EVP_PKEY_get_raw_public_key(pkey, NULL, &publen);
    cout << "Public Key length: " << publen << "\n";
    //EVP_PKEY_new_raw_private_key(); // Imports

    if (ctx == NULL) {
        cout << "fuck\n";
    }

    cout << "innit status: " << EVP_PKEY_encapsulate_init(ctx, NULL) << "\n";
    
    
    EVP_PKEY_encapsulate(ctx, NULL, &outlen, NULL, &secretlen);




    unsigned char out[outlen], secret[secretlen];
    cout << "status: " << EVP_PKEY_encapsulate(ctx, out, &outlen, secret, &secretlen) << "\n";
    cout << "OutLen: " << outlen << "\n";

    FILE *a;
    a = fopen("KEM-ML_Secret.bin", "wb");
    fwrite(secret, 1, secretlen, a);
    fclose(a);

    FILE *b;
    b = fopen("KEM-ML_Output.bin", "wb");
    fwrite(out, 1, outlen, b);
    fclose(b);


    cout << "Dencapsulate Innit: " << EVP_PKEY_decapsulate_init(ctx, NULL) << "\n";

    size_t sLen;
    cout << "Dencapsulate Size Check: " << EVP_PKEY_decapsulate(ctx, NULL, &sLen, out, outlen) << "\n";
    unsigned char sharedSecret[sLen];
    cout << sLen << "\n";
    EVP_PKEY_decapsulate(ctx, sharedSecret, &sLen, out, outlen);

    FILE *c;
    c = fopen("KEM-ML_Secret2.bin", "wb");
    fwrite(sharedSecret, 1, sLen, c);
    fclose(c);

    

    //for (int i = 0; i < sLen; i++) {
    //     std::cout << sharedSecret[i];
    //}
    cout << "\n";
    cout << "Done!\n";
}
