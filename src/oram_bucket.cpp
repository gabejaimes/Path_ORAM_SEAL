#include "../include/oram_bucket.h"
#include <utility>

// default constructor just resizes vector to proper size
ORAMBucket::ORAMBucket() {
    encryptedBlocks.resize(getCapacity());
}

// this constructor generates a new block based on already generate bucketBlocks
ORAMBucket::ORAMBucket(std::vector<std::vector<unsigned char> > bucketBlocks)
    : encryptedBlocks(bucketBlocks) {}

// Getters
std::vector<std::vector<unsigned char>> ORAMBucket::getEncryptedBlocks() const {
    return encryptedBlocks;
}

size_t ORAMBucket::getCapacity() {
    return bucketCapacity;
}

// Modifiers

void ORAMBucket::setBlocks(const std::vector<std::vector<unsigned char>> &newBlocks) {
    encryptedBlocks = newBlocks;
}


