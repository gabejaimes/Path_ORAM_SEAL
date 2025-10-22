#ifndef PATHORAM_ORAM_TREE_H
#define PATHORAM_ORAM_TREE_H

#include "oram_bucket.h"
#include <vector>
#include <cmath>
#include <utility>

// create structure for oram tree storing
// height, numLeaves, numBuckets, numDataBlocks, and bucketCapacity
class ORAMTree {
private:
    std::vector<ORAMBucket> tree;
    int height;
    int numLeaves;
    int numBuckets;
    int numDataBlocks;
    size_t bucketCapacity;

    // private function to get path indices from leaf to root
    [[nodiscard]] std::vector<int> getPathIndices(int leaf) const;

public:
    // Constructor
    ORAMTree(int numDBlocks, std::vector<ORAMBucket> initBlocks);
    ORAMTree() = default;


    // Getters
    [[nodiscard]] int getHeight() const;
    [[nodiscard]] int getNumBuckets() const;
    [[nodiscard]] int getNumLeaves() const;
    [[nodiscard]] size_t getBucketCapacity() const;
    [[nodiscard]] int getNumDataBlocks() const;
    [[nodiscard]] ORAMBucket& getBucket(int index);
    // fetch path indices and buckets along path from leaf node to root
    [[nodiscard]] std::pair<std::vector<int>, std::vector<ORAMBucket> > fetchPath(int leafNumber) const;
    // a helper fucntion to write back the path to the oram tree
    void writeBackPath(const std::vector<int>& pathIndices, const std::vector<ORAMBucket>& modifiedPath);




};


#endif //PATHORAM_ORAM_TREE_H
