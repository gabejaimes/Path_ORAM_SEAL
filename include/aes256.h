#ifndef PATHORAM_AES256_H
#define PATHORAM_AES256_H

#include <vector>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <memory>


// Fix AES block size and key size
const int AES_BLOCK_SIZE = 16;
const int AES_KEY_SIZE = 32;

class AES256 {
public:
    // AES constructor
    AES256();

    // generate a new key
    static std::vector<unsigned char> generateKey();
    // encrypt a vector of unsigned chars (raw binary data) with associated key
    static std::vector<unsigned char> encrypt(const std::vector<unsigned char>& plaintext, const unsigned char* key);
    // decrypt a vector of unsigned chars (raw binary data) with associated key
    static std::vector<unsigned char> decrypt(const std::vector<unsigned char>& ciphertext, const unsigned char* key);
};


#endif //PATHORAM_AES256_H
