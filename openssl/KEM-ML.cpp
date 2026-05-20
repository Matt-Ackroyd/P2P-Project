#include <openssl/evp.h>
#include <iostream>

using namespace std;




int main() {
    EVP_PKEY_CTX *ctx = NULL;
    size_t secretlen = 0, outlen = 0;
    

    
    EVP_PKEY *pkey = EVP_PKEY_Q_keygen(NULL, NULL, "ML-KEM-1024");

    ctx = EVP_PKEY_CTX_new_from_pkey(NULL, pkey, NULL);


    EVP_PKEY_get_raw_public_key();  // Exports?
    EVP_PKEY_new_raw_private_key(); // Imports
    EVP_PKEY_export(pkey);

    if (ctx == NULL) {
        cout << "fuck\n";
    }

    cout << "innit status: " << EVP_PKEY_encapsulate_init(ctx, NULL) << "\n";
    
    
    EVP_PKEY_encapsulate(ctx, NULL, &outlen, NULL, &secretlen);




    unsigned char out[outlen], secret[secretlen];
    cout << "status: " << EVP_PKEY_encapsulate(ctx, out, &outlen, secret, &secretlen) << "\n";
    cout << outlen << "\n";

    FILE *a;
    a = fopen("KEM-ML_Secret.bin", "wb");
    fwrite(secret, 1, secretlen, a);
    fclose(a);

    FILE *b;
    b = fopen("KEM-ML_Output.bin", "wb");
    fwrite(out, 1, outlen, b);
    fclose(b);

    

    // for (int i = 0; i < secretlen; i++) {
    //     std::cout << secret[i];
    // }
    cout << "\n";
    cout << "Done!\n";
}
