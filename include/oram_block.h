#ifndef PATHORAM_ORAM_BLOCK_H
#define PATHORAM_ORAM_BLOCK_H

#include <string>
#include <utility>
#include <vector>
#include <cstring>
#include <openssl/rand.h>

// create oram block which stores blockID, mappedLead, lenDocIDs, maxDocIDs, and finally data which are docIDs
class ORAMBlock {
private:
    int blockID;
    int mappedLeaf;
    int lenDocIDS;
    int maxDocIDs;
    std::vector<int> docIDs;

    // create a function to generate a random number for initializing dummy blocks
    static int randNumInRange(int n);
public:
    // Constructors
    // dummy constructor
    explicit ORAMBlock(int maxSize);
    // real constructor
    ORAMBlock(int blockID, int mappedLeaf, int maxDocIDS, const std::vector<int>& data);

    // impelement equality checks to see if two blocks are equal and not equal
    bool operator==(const ORAMBlock& other) const;
    bool operator!=(const ORAMBlock& other) const;

    // Getters to get info related to block
    [[nodiscard]] int getBlockID() const;
    [[nodiscard]] int getMappedLeaf() const;
    [[nodiscard]] std::vector<int> getDocIDs() const;

    // Setters to set info related to a block
    void setDocIDs(const std::vector<int>& newData);
    void setDocIDs(std::vector<int>&& newData);
    void setMappedLeaf(int newLeaf);

    // function to check if block is dummy
    [[nodiscard]] bool isDummy() const;

    // serializer
   [[nodiscard]] std::vector<unsigned char> serialize() const;

    // deserializer
    static ORAMBlock deserialize(const std::vector<unsigned char> &byteArray);


};

#endif //PATHORAM_ORAM_BLOCK_H
