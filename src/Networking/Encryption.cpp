#include "Encryption.h"
#include <filesystem>
#include "ConfigLoader.h"

// uses a GCM block cipher to encrypt plaintext into the ciphertext, returns the ciphertext length
int symmetricEncryption(unsigned char *plaintext, int plaintext_len,
                unsigned char *aad, int aad_len,
                unsigned char *key,
                unsigned char *iv, int iv_len,
                unsigned char *ciphertext,
                unsigned char *tag) {
    EVP_CIPHER_CTX *ctx;
    int ciphertext_len;
    int len;
    

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    /* Initialise the encryption operation. */
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL))
        handleErrors();

    /*
     * Set IV length if default 12 bytes (96 bits) is not appropriate
     */
    if(1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, AES_256_IV_LENGTH, NULL))
        handleErrors();

    /* Initialise key and IV */
    if(1 != EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv))
        handleErrors();

    /*
     * Provide any AAD data. This can be called zero or more times as
     * required
     */
    if(1 != EVP_EncryptUpdate(ctx, NULL, &len, aad, aad_len))
        handleErrors();

    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleErrors();
    ciphertext_len = len;

    /*
     * Finalise the encryption. Normally ciphertext bytes may be written at
     * this stage, but this does not occur in GCM mode
     */
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        handleErrors();
    ciphertext_len += len;

    /* Get the tag */
    if(1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, AES_256_GCM_TAG_LENGTH, tag))
        handleErrors();

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

int symmetricDecryption(unsigned char *ciphertext, int ciphertext_len,
                unsigned char *aad, int aad_len,
                unsigned char *tag,
                unsigned char *key,
                unsigned char *iv, int iv_len,
                unsigned char *plaintext)
{
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;
    int ret;

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    /* Initialise the decryption operation. */
    if(!EVP_DecryptInit_ex2(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL))
        handleErrors();

    /* Set IV length. Not necessary if this is 12 bytes (96 bits) */
    if(!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv_len, NULL))
        handleErrors();

    /* Initialise key and IV */
    if(!EVP_DecryptInit_ex2(ctx, NULL, key, iv, NULL))
        handleErrors();

    /*
     * Provide any AAD data. This can be called zero or more times as
     * required
     */
    if(!EVP_DecryptUpdate(ctx, NULL, &len, aad, aad_len))
        handleErrors();

    /*
     * Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary
     */
    if(!EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleErrors();
    plaintext_len = len;

    /* Set expected tag value. Works in OpenSSL 1.0.1d and later */
    if(!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, tag))
        handleErrors();

    /*
     * Finalise the decryption. A positive return value indicates success,
     * anything else is a failure - the plaintext is not trustworthy.
     */
    ret = EVP_DecryptFinal_ex(ctx, plaintext + len, &len);

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    if(ret > 0) {
        /* Success */
        plaintext_len += len;
        return plaintext_len;
    } else {
        /* Verify failed */
        return -1;
    }
}

int shaw256Hash(unsigned char* input, int inputlen, unsigned char* shaw256output) {
    EVP_MD_CTX *ctx = NULL;
    EVP_MD *sha256 = NULL;

    // only has to be done once TODO MOVE OUTSIDE
    // New ctx for hash
    ctx = EVP_MD_CTX_new();
    if (ctx == NULL)
        handleErrors();

    // Fetch the shaw alg
    sha256 = EVP_MD_fetch(NULL, "SHA256", NULL);
    if (sha256 == NULL)
        handleErrors();


    
    // innit
    if (!EVP_DigestInit_ex(ctx, sha256, NULL))
       handleErrors();

    // Hash
    if (!EVP_DigestUpdate(ctx, input, inputlen))
        handleErrors();
    
    // Hash output
    unsigned int len;
    if (!EVP_DigestFinal_ex(ctx, shaw256output, &len))
        handleErrors();
    return 1;
}

void handshakeHash(unsigned char* premaster, int premasterlen, unsigned char* rand1, unsigned char* rand2, unsigned char* output) { 
    std::string saltStr = "Handshake";

    int hashInputlen = premasterlen + 2*ML_KEM_HANDSHAKE_RANDSIZE + saltStr.length()+1;
    unsigned char hashInput[hashInputlen];

    memcpy(hashInput, premaster, premasterlen);
    int offset = premasterlen;

    memcpy(hashInput+offset, rand1, ML_KEM_HANDSHAKE_RANDSIZE);
    offset += ML_KEM_HANDSHAKE_RANDSIZE;

    memcpy(hashInput+offset, rand2, ML_KEM_HANDSHAKE_RANDSIZE);
    offset += ML_KEM_HANDSHAKE_RANDSIZE;

    memcpy(hashInput+offset, (unsigned char*)saltStr.c_str(), saltStr.length()+1);

    shaw256Hash(hashInput, hashInputlen, output);
}


void handleErrors() {
    throw std::runtime_error("Encrypt/Decrypt Failure\n");
}


EVP_PKEY* getDSAkeyFromFile() {
    EVP_PKEY* pkey;

    std::filesystem::path path("Configs/PrimaryClient/DSAkey.bin");
    if (!std::filesystem::exists(path)) {
        EVP_PKEY_CTX *pctx = EVP_PKEY_CTX_new_from_name(NULL, "ML-DSA-87", NULL);
        pkey = EVP_PKEY_Q_keygen(NULL, NULL, "ML-DSA-87");

        unsigned char privateKey[4896];

        size_t len;
        EVP_PKEY_get_raw_private_key(pkey, NULL, &len);
        EVP_PKEY_get_raw_private_key(pkey, privateKey, &len);
        ConfigLoader::WriteBinaryFile("Configs/PrimaryClient/DSAkey.bin", (char*)privateKey, len);
        
    } else {    

        unsigned char rawprivateKey[4896];
        ConfigLoader::ReadBinaryFile("Configs/PrimaryClient/DSAkey.bin", (char*)rawprivateKey, 4896);

        pkey = EVP_PKEY_new_raw_private_key_ex(NULL, "ML-DSA-87", NULL, rawprivateKey, ML_DSA_87_PRIVATE_KEY_BYTE_SIZE);
    }
    return pkey;
}


void signMessage(EVP_PKEY *key, unsigned char *msg, size_t msg_len, unsigned char* signatureBuffer) {
    size_t sig_len;

    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_from_pkey(NULL, key, NULL);
    EVP_SIGNATURE *sig_alg = EVP_SIGNATURE_fetch(NULL, "ML-DSA-87", NULL);

    EVP_PKEY_sign_message_init(ctx, sig_alg, NULL);
    EVP_PKEY_sign(ctx, signatureBuffer, &sig_len, msg, msg_len);

    EVP_SIGNATURE_free(sig_alg);
    EVP_PKEY_CTX_free(ctx);
}


bool verifyMessage(EVP_PKEY *key, unsigned char* sig, unsigned char *msg, size_t msg_len) {
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_from_pkey(NULL, key, NULL);
    EVP_SIGNATURE *sig_alg = EVP_SIGNATURE_fetch(NULL, "ML-DSA-87", NULL);

    EVP_PKEY_verify_message_init(ctx, sig_alg, NULL);

    int ret = EVP_PKEY_verify(ctx, sig, ML_DSA_87_SIGNATURE_BYTE_SIZE, msg, msg_len);

    EVP_SIGNATURE_free(sig_alg);
    EVP_PKEY_CTX_free(ctx);

    if (ret == 1) {
        return true;
    }
    return false;
}


void signFile(EVP_PKEY *key, std::string filePath, unsigned char* signatureBuffer) {
    std::filesystem::path path(filePath);

    if (!std::filesystem::exists(path)) {
        return;
    }

    size_t sig_len = ML_DSA_87_SIGNATURE_BYTE_SIZE;

    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_from_pkey(NULL, key, NULL);
    EVP_SIGNATURE *sig_alg = EVP_SIGNATURE_fetch(NULL, "ML-DSA-87", NULL);

    EVP_PKEY_sign_message_init(ctx, sig_alg, NULL);

    char buffer[1024];
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return;
    }
    
    while (int inlen = file.readsome(buffer, 1024) > 0) {
        EVP_PKEY_sign_message_update(ctx, (unsigned char*)buffer, inlen);
    }

    EVP_PKEY_sign_message_final(ctx, signatureBuffer, &sig_len);

    EVP_SIGNATURE_free(sig_alg);
    EVP_PKEY_CTX_free(ctx);

}


bool verifyFile(EVP_PKEY *key, unsigned char* sig, std::string filePath) {
    std::filesystem::path path(filePath);

    if (!std::filesystem::exists(path)) {
        return false;
    }

    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_from_pkey(NULL, key, NULL);
    EVP_SIGNATURE *sig_alg = EVP_SIGNATURE_fetch(NULL, "ML-DSA-87", NULL);

    EVP_PKEY_verify_message_init(ctx, sig_alg, NULL);

    EVP_PKEY_CTX_set_signature(ctx, sig, ML_DSA_87_SIGNATURE_BYTE_SIZE);


    char buffer[1024];
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    while (int inlen = file.readsome(buffer, 1024) > 0) {
        EVP_PKEY_verify_message_update(ctx, (unsigned char*)buffer, inlen);
    }

    int ret = EVP_PKEY_verify_message_final(ctx);

    EVP_SIGNATURE_free(sig_alg);
    EVP_PKEY_CTX_free(ctx);

    if (ret == 1) {
        return true;
    }
    return false;
}