#ifndef PATHORAM_POSITION_MAP_H
#define PATHORAM_POSITION_MAP_H

#include <unordered_map>
#include <openssl/rand.h>
#include <stdexcept>

// create position map class to store unordered map mapping block id to leaf node, and numLeaves and numBlocks in map
class PositionMap {
private:
    std::unordered_map<int, int> positionMap;
    int numLeaves;
    int numBlocks;

    // create a function to generate a random leaf number when updating leaf num in postion map
    [[nodiscard]] int randomLeafNum() const;


public:
    // Constructor
    PositionMap(int numBlocks, int numLeaves);
    PositionMap() = default;

    // Get position of block ID in map
    [[nodiscard]] int getPosition(int blockID);


    // assign a new leaf number to the blockID
    [[nodiscard]] int assignNewLeaf(int blockID);

};


#endif //PATHORAM_POSITION_MAP_H
