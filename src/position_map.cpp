#include "../include/position_map.h"
#include <iostream>

// for each block id init it to a random leaf
PositionMap::PositionMap(int numBlocks, int numLeaves): numBlocks(numBlocks), numLeaves(numLeaves) {
    for( int blockID = 0; blockID < numBlocks; blockID++ ) {
        positionMap[blockID] = randomLeafNum();
    }
}

// simply see what position is mapped to the block id and return it
int PositionMap::getPosition(int blockID) {
    return positionMap[blockID];
}

// use rand bytes to generate a random leaf number
int PositionMap::randomLeafNum() const {
    unsigned char buf[4];
    if (RAND_bytes(buf, sizeof(buf)) != 1) {
        throw std::runtime_error("OpenSSL RAND_bytes failed");
    }

    int randomValue = (buf[0] << 24) |
                      (buf[1] << 16) |
                      (buf[2] << 8)  |
                      (buf[3]);

    return std::abs(randomValue) % numLeaves;
}

// assign a new random leaf to block id and return said leaf
int PositionMap::assignNewLeaf(int blockID) {
    int newLeaf = randomLeafNum();
    positionMap[blockID] = newLeaf;
    return newLeaf;
}
