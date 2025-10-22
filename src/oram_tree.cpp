#include "../include/oram_tree.h"
#include <iostream>
#include "iostream"

// constructor for oram tree initializing all values
ORAMTree::ORAMTree(int numDBlocks, std::vector<ORAMBucket> initBlocks) {
    numDataBlocks = numDBlocks;
    numLeaves = static_cast<int>(std::pow(2 ,std::ceil(std::log2(numDataBlocks))));
    numBuckets = 2 * numLeaves - 1;
    height = std::log2(numLeaves);
    tree = initBlocks;
    bucketCapacity = ORAMBucket::getCapacity();
}

// Getters

int ORAMTree::getHeight() const {
    return height;
}

int ORAMTree::getNumLeaves() const {
    return numLeaves;
}

int ORAMTree::getNumBuckets() const {
    return numBuckets;
}

size_t ORAMTree::getBucketCapacity() const {
    return bucketCapacity;
}

ORAMBucket& ORAMTree::getBucket(int index) {
    return tree[index];
}

int ORAMTree::getNumDataBlocks() const {
    return numDataBlocks;
}

// traverse from leaf node to root to get the path indices
std::vector<int> ORAMTree::getPathIndices(int leafNumber) const {
    std::vector<int> path;
    int index = (numBuckets - numLeaves) + leafNumber;

    while( index > 0 ) {
        path.push_back(index);
        index = (index - 1) / 2;
    }
    path.push_back(0);
    return path;
}

// fetch path by using get path indices and for each path index fetch the bucket at that index and add it to a vector
// to return
std::pair<std::vector<int>, std::vector<ORAMBucket> > ORAMTree::fetchPath(int leafNumber) const {
    std::vector<int> pathIndices = getPathIndices(leafNumber);

    std::vector<ORAMBucket> pathBuckets;

    for( auto index : pathIndices ) {
        pathBuckets.push_back(tree[index]);
    }

    return {pathIndices, pathBuckets};
}

// simply write back the bucket to the specified path
void ORAMTree::writeBackPath(const std::vector<int> &pathIndices, const std::vector<ORAMBucket> &modifiedPath) {
    for( size_t i = 0; i < pathIndices.size(); i++ ) {
        tree[pathIndices[i]] = modifiedPath[i];
    }
}

