#include <openssl/evp.h>
#include <iostream>
using namespace std;

//#define AES_256_KEY_LENGTH      32
//#define AES_256_KEY_LENGTH_BITS 256
#define AES_256_IV_LENGTH     16
#define AES_256_GCM_TAG_LENGTH  16


int symmetricEncryption();
int symmetricDecryption();

void handleErrors() {
    cout << "We Messed Up\n";
    exit(1);
}