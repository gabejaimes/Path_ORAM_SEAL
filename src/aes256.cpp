#include "../include/aes256.h"

// default constructor
AES256::AES256() {}

// generate key using rand bytes and return said key
std::vector<unsigned char> AES256::generateKey() {
    std::vector<unsigned char> key(AES_KEY_SIZE);
    if (RAND_bytes(key.data(), AES_KEY_SIZE) != 1) {
        throw std::runtime_error("OpenSSL RAND_bytes failed");
    }
    return key;
}

std::vector<unsigned char> AES256::encrypt(const std::vector<unsigned char> &plaintext, const unsigned char *key) {
    // create cipher context
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if(!ctx) {
        throw std::runtime_error("OpenSSL EVP_CIPHER_CTX failed");
    }

    // create IV
    unsigned char iv[AES_BLOCK_SIZE];
    if (RAND_bytes(iv, AES_BLOCK_SIZE) != 1) {
        throw std::runtime_error("OpenSSL RAND_bytes failed");
    }

    // Initialize AES-256-CBC enc with key and IV
    if(EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv) != 1) {
        throw std::runtime_error("OpenSSL EVP_EncryptInit_ex failed");
    }

    // create buffer for ciphertext to accommodate padding
    std::vector<unsigned char> ciphertext(plaintext.size() + AES_BLOCK_SIZE);

    // Create vars to keep track of ciphertext len
    int bytes_written, ciphertext_len = 0;

    // Encrypt plaintext
    if (EVP_EncryptUpdate(ctx, ciphertext.data(), &bytes_written, plaintext.data(), plaintext.size()) != 1) {
        throw std::runtime_error("OpenSSL EVP_EncryptUpdate failed");
    }
    // update ciphertext len
    ciphertext_len += bytes_written;

    // finalize enc starting at the end of the ciphertext buffer to put in right place
    if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + ciphertext_len, &bytes_written) != 1) {
        throw std::runtime_error("OpenSSL EVP_EncryptFinal_ex failed");
    }
    // update len again
    ciphertext_len += bytes_written;

    // resize ciphertext to actual size
    ciphertext.resize(ciphertext_len);

    // free context
    EVP_CIPHER_CTX_free(ctx);

    // prepend the iv to ciphertext
    std::vector<unsigned char> encrypted(iv, iv + AES_BLOCK_SIZE);
    // then add ciphertext
    encrypted.insert(encrypted.end(), ciphertext.begin(), ciphertext.end());
    return encrypted;
}

std::vector<unsigned char> AES256::decrypt(const std::vector<unsigned char> &encryption, const unsigned char *key) {
    // create context
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if(ctx == NULL) {
        throw std::runtime_error("OpenSSL EVP_CIPHER_CTX failed");
    }

    // create space for iv
    unsigned char iv[AES_BLOCK_SIZE];
    // copy it from beginning of ciphertext
    std::memcpy(iv, encryption.data(), AES_BLOCK_SIZE);

    // create vector and extract ciphertext from encryption, starting after iv all the way to end
    std::vector<unsigned char> ciphertext(encryption.begin() + AES_BLOCK_SIZE, encryption.end());

    // Initialize AES-256-CBC dec with key and IV
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv) != 1) {
        throw std::runtime_error("OpenSSL EVP_DecryptInit_ex failed");
    }

    // create vector for plaintext
    std::vector<unsigned char> plaintext(ciphertext.size());

    // Create vars to keep track of ciphertext len
    int bytes_written, plaintext_len = 0;

    // Decrypt ciphertext
    if (EVP_DecryptUpdate(ctx, plaintext.data(), &bytes_written, ciphertext.data(), ciphertext.size()) != 1) {
        throw std::runtime_error("OpenSSL EVP_DecryptUpdate failed");
    }

    // update plaintext len
    plaintext_len += bytes_written;

    // finalize dec starting at end of plaintext already dec
    if (EVP_DecryptFinal_ex(ctx, plaintext.data() + plaintext_len, &bytes_written) != 1) {
        throw std::runtime_error("OpenSSL EVP_DecryptFinal_ex failed");
    }
    // update len
    plaintext_len += bytes_written;

    // resize plaintext to actual size
    plaintext.resize(plaintext_len);

    // free context
    EVP_CIPHER_CTX_free(ctx);

    return plaintext;
}