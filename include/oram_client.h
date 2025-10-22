#ifndef PATHORAM_ORAM_CLIENT_H
#define PATHORAM_ORAM_CLIENT_H

#include "oram_tree.h"
#include "position_map.h"
#include <vector>
#include <algorithm>
#include <string>
#include "aes256.h"

// create oram client to use oram
class ORAMClient {
private:
    // stores oram tree, position map, stash, key for encrypting, and maxDocID count
    ORAMTree oramTree;
    PositionMap positionMap;
    std::vector<ORAMBlock> stash;
    std::vector<unsigned char> key;
    int maxDocIDs;

    // Given a path, noted by the leaf number calculates what is the index of the bucket along the path at height l
   int P(int leafNum, int l);
   // given a list of buckets decrypts and deserializes and aggregates all real blocks while removing dummy blocks
   std::vector<ORAMBlock> aggregateNonDummyBlocks(const std::vector<ORAMBucket>& buckets);
   // given a level and leaf number, filters blocks in the stash to blocks that can be placed in the bucket along path
   // noted by leaf number at level 'level'
   std::vector<ORAMBlock> filterBlocksForLevel(int oldLeafNumber, int level);
   // once all blocks are in stash loops over all the blocks looking for blockID block, if it is in there then it
   // removes the block from the stash and adds a new block changing the mapped leaf to the newleaf number and modifies
   // data if it is a write, otherwise it stays the same
   ORAMBlock findAndUpdateStashBlock(int blockID, int newLeafNumber,
                                     const std::vector<int> &newData, bool isWrite);
   // given a list of blocks it will serialize and then encrypt all of them returning them as a vector of a vector
   // of unsigned chars
   std::vector<std::vector<unsigned char> > serializeAndEncryptBlocks(const std::vector<ORAMBlock> &blocks);
   // creates encrypted dummyBuckets to initalize the ORAM tree with based on number of blocks we intend to store
   std::vector<ORAMBucket> createEncryptedDummyBuckets(int blocksToStore);

public:
    // constructor for ORAM client
    ORAMClient(int blocksToStore, int maxDocCount);

    // operation to access data from the ORAM tree
    std::vector<int> accessBlock(int blockID, bool isWrite, const std::vector<int>& newData);

    // function to evict path denoted by oldLeafNumber and write back as many blocks as we can back to the
    // path while encrypting again to maintain obliviousness
    void evictPath(int oldLeafNumber);

};


#endif //PATHORAM_ORAM_CLIENT_H
