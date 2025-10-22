#include "../include/oram_block.h"

// Default constructor for a dummy block making docIDs random so all dummy blocks don't look identical
ORAMBlock::ORAMBlock(int maxSize): maxDocIDs(maxSize), blockID(-1), mappedLeaf(-1){
    lenDocIDS = randNumInRange(maxSize);
    docIDs.resize(lenDocIDS);

}

// Constructor for valid block
ORAMBlock::ORAMBlock(int blockID, int mappedLeaf, int maxDocIDs, const std::vector<int>& data) :
        blockID(blockID),maxDocIDs(maxDocIDs), mappedLeaf(mappedLeaf), docIDs(data), lenDocIDS(data.size()) {}

// implement equality checks
bool ORAMBlock::operator==(const ORAMBlock &other) const {
    return blockID == other.blockID && mappedLeaf == other.mappedLeaf;
}

bool ORAMBlock::operator!=(const ORAMBlock &other) const {
    return !(*this == other);
}

// Getters
int ORAMBlock::getBlockID() const {
    return blockID;
}

int ORAMBlock::getMappedLeaf() const {
    return mappedLeaf;
}

std::vector<int> ORAMBlock::getDocIDs() const {
    return docIDs;
}

// Setters

// L value, automatically copied
void ORAMBlock::setDocIDs(const std::vector<int>& newData) {
    docIDs = newData;
}

// R value
void ORAMBlock::setDocIDs(std::vector<int> &&newData) {
    docIDs = std::move(newData);
}

// change mapped leaf
void ORAMBlock::setMappedLeaf(int newLeaf) {
    mappedLeaf = newLeaf;
}

// check to see if block is dummy or not
bool ORAMBlock::isDummy() const {
    return blockID == -1;
}

// serialize
std::vector<unsigned char> ORAMBlock::serialize() const {
    size_t totalSize = sizeof(blockID) + sizeof(mappedLeaf) + sizeof(lenDocIDS) + sizeof(maxDocIDs) + (docIDs.size() * sizeof(int));
    // arr for block bytes
    std::vector<unsigned char> byteArray(totalSize);

    size_t offset = 0;

    // copy to byteArray
    std::memcpy(byteArray.data() + offset, &blockID, sizeof(blockID));
    offset += sizeof(blockID);

    std::memcpy(byteArray.data() + offset, &mappedLeaf, sizeof(mappedLeaf));
    offset += sizeof(mappedLeaf);

    std::memcpy(byteArray.data() + offset, &lenDocIDS, sizeof(lenDocIDS));
    offset += sizeof(lenDocIDS);


    std::memcpy(byteArray.data() + offset, &maxDocIDs, sizeof(maxDocIDs));
    offset += sizeof(maxDocIDs);

    // copy all docIDs into array
    for(int docID : docIDs) {
        std::memcpy(byteArray.data() + offset, &docID, sizeof(docID));
        offset += sizeof(int);
    }

    return byteArray;
}

ORAMBlock ORAMBlock::deserialize(const std::vector<unsigned char> &byteArray) {
    size_t offset = 0;

    // read each value from the byte array and construct new block based off of it
    int blockID;
    std::memcpy(&blockID, byteArray.data() + offset, sizeof(blockID));
    offset += sizeof(blockID);

    int mappedLeaf;
    std::memcpy(&mappedLeaf, byteArray.data() + offset, sizeof(mappedLeaf));
    offset += sizeof(mappedLeaf);

    int lenDocIDs;
    std::memcpy(&lenDocIDs, byteArray.data() + offset, sizeof(lenDocIDs));
    offset += sizeof(lenDocIDs);

    int docIDSize;
    std::memcpy(&docIDSize, byteArray.data() + offset, sizeof(docIDSize));
    offset += sizeof(docIDSize);

    // including all docIDs
    std::vector<int> docIDs(lenDocIDs);
    for (int i = 0; i < lenDocIDs; i++) {
        std::memcpy(&docIDs[i], byteArray.data() + offset, sizeof(int));
        offset += sizeof(int);
    }

    // change it to ORAMBlock form
    return ORAMBlock(blockID, mappedLeaf, docIDSize ,docIDs);
}

// use rand to create a random number in range [0, n)
int ORAMBlock::randNumInRange(int n) {
        unsigned char buf[4];
        if (RAND_bytes(buf, sizeof(buf)) != 1) {
            throw std::runtime_error("OpenSSL RAND_bytes failed");
        }

        int randomValue = (buf[0] << 24) |
                          (buf[1] << 16) |
                          (buf[2] << 8)  |
                          (buf[3]);

        return std::abs(randomValue) % n;
}





