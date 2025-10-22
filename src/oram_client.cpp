#include "../include/oram_client.h"
#include "iostream"

// constructor to initialize oram client
ORAMClient::ORAMClient(int blocksToStore, int maxDocCount) {
    key = AES256::generateKey();
    maxDocIDs = maxDocCount;
    oramTree = ORAMTree(blocksToStore, createEncryptedDummyBuckets(blocksToStore));
    positionMap = PositionMap(blocksToStore, oramTree.getNumLeaves());
}

std::vector<int> ORAMClient::accessBlock(int blockID, bool isWrite, const std::vector<int> &newData) {
    if (blockID < 0 || blockID >= oramTree.getNumDataBlocks()) {
        throw std::out_of_range("Block ID " + std::to_string(blockID) + " is out of range.");
    }

    // Create var to save found block initializing it to Dummy
    ORAMBlock foundBlock = ORAMBlock(maxDocIDs);

    // Save old leaf number from position map
    int oldLeafNumber = positionMap.getPosition(blockID);


    // if -1 then throw error
    if (oldLeafNumber == -1) throw std::runtime_error("Block not found in Position Map");

    // assign block a new leaf and save it
    int newLeafNumber = positionMap.assignNewLeaf(blockID);

    // fetch pathIndices and Buckets from P(x)
    auto [pathIndices, encryptedBuckets] = oramTree.fetchPath(oldLeafNumber);

    // Get all nonDummyBlocks from path and search for target block in it while dec and deserializing
    std::vector<ORAMBlock> nonDummyBlocks = aggregateNonDummyBlocks(encryptedBuckets);


    // add blocks to the stash
    stash.insert(stash.begin(), nonDummyBlocks.begin(), nonDummyBlocks.end());

    // find and update stash block if necessary
    foundBlock = findAndUpdateStashBlock(blockID, newLeafNumber, newData, isWrite);

    // if not found, and we have a write then insert into stash to be properly inserted
    if (foundBlock.isDummy() && isWrite) {
        foundBlock = ORAMBlock(blockID, newLeafNumber, maxDocIDs, newData);  // Create new block
        stash.push_back(foundBlock);  // Add it to stash
    }


    // run eviction protocol
    evictPath(oldLeafNumber);

    return foundBlock.getDocIDs();
}

// calculates which bucket is at level l in the path denoted by leafNum by starting at leaf num and jumping up L - l
// levels
int ORAMClient::P(int leafNum, int l) {
    int L = oramTree.getHeight();
    int index = (1 << L) - 1 + leafNum;
    for(int i = 0; i < L-l; i++) {
        index = (index - 1) / 2;
    }
    return index;
}

std::vector<ORAMBlock> ORAMClient::aggregateNonDummyBlocks(const std::vector<ORAMBucket> &buckets) {
    std::vector<ORAMBlock> realBlocks;

    for(const auto& bucket : buckets) {
        for(const auto& encryptedBlock : bucket.getEncryptedBlocks()) {
            // decrypt block
            std::vector<unsigned char> decryptedData = AES256::decrypt(encryptedBlock, key.data());
            // deserialize into block
            ORAMBlock block = ORAMBlock::deserialize(decryptedData);
            if( !block.isDummy() ) {
                realBlocks.push_back(block);
            }
        }
    }

    return realBlocks;
}

std::vector<ORAMBlock>
ORAMClient::filterBlocksForLevel(int oldLeafNumber, int level) {
    std::vector<ORAMBlock> filteredBlocks;

    // begins at beginning of stash and loops until end
    for( auto it = stash.begin(); it != stash.end();) {
        // get iterator leaf num
        int itLeafNum = it->getMappedLeaf();
        // if P(intLeafNum, level) is equal to bucket of P(oldLeafNumber, level), then we can place the block there
        if(P(itLeafNum, level) == P(oldLeafNumber, level)) {
            // so push to filtered blocks
            filteredBlocks.push_back(*it);
            // erase block from stash
            it = stash.erase(it);
            // and don't update iterator since we just deleted
        }
        else {
            // otherwise update iterator to carry on since we didn't delete
            it++;
        }
    }

    // finally return filtered blocks
    return filteredBlocks;
}

ORAMBlock ORAMClient::findAndUpdateStashBlock(int blockID, int newLeafNumber,
                                              const std::vector<int> &newData, bool isWrite) {
    ORAMBlock foundBlock = ORAMBlock(maxDocIDs);
    // loop over stash to find block
    for (auto it = stash.begin(); it != stash.end(); ++it) {
        // if we find it
        if (it->getBlockID() == blockID) {
            // save it
            foundBlock = *it;
            // and if write erase from stash since we are going to be changing it
            stash.erase(it);
            if(isWrite) {
                // now add updated block
                stash.emplace_back(blockID, newLeafNumber, maxDocIDs, newData);
            }
            else {
                // or just update leaf number
                stash.emplace_back(blockID, newLeafNumber, maxDocIDs, foundBlock.getDocIDs());
            }
            // stop looping since found and we have done what's needed
            break;
        }
    }
    return foundBlock;
}

std::vector<std::vector<unsigned char>> ORAMClient::serializeAndEncryptBlocks(const std::vector<ORAMBlock> &blocks) {
    std::vector<std::vector<unsigned char>> encryptedBlocks;

    for( const auto &block : blocks) {
        // serialize
        std::vector<unsigned char> serializedBlock = block.serialize();
        // encrypt
        std::vector<unsigned char> encryptedBlock = AES256::encrypt(serializedBlock, key.data());
        // store encrypted block
        encryptedBlocks.push_back(encryptedBlock);
    }
    return encryptedBlocks;
}

void ORAMClient::evictPath(int oldLeafNumber) {
    // get height of tree
    int L = oramTree.getHeight();

    // create vector to store new path
    std::vector<ORAMBucket> newPath;

    // get path indices along eviction path
    std::vector<int> pathIndices = oramTree.fetchPath(oldLeafNumber).first;

    // start from leaf node
    for(int l = L; l >= 0; l--) {
        // get S', all the blocks that can be written to this level
        std::vector<ORAMBlock> stashPrime = filterBlocksForLevel(oldLeafNumber, l);

        // Then select min(|S'|, Z) blocks to write to the bucket
        size_t blocksToSelect = std::min(stashPrime.size(), oramTree.getBucketCapacity());

        // Get said blocks
        std::vector<ORAMBlock> newBucketBlocks(stashPrime.begin(), stashPrime.begin() + blocksToSelect);

        // erase blocks taken from stash'
        stashPrime.erase(stashPrime.begin(), stashPrime.begin() + blocksToSelect);

        // now we need to add the blocks not taken from stash' back to the stash
        stash.insert(stash.end(), stashPrime.begin(), stashPrime.end());

        // now we need to pad the newBuckBlocks if we don't have bucketCapacity blocks in it
        while(newBucketBlocks.size() < oramTree.getBucketCapacity()) {
            newBucketBlocks.emplace_back(maxDocIDs);
        }

        // serialize and encrypt blocks
        std::vector<std::vector<unsigned char>> encryptedBucket = serializeAndEncryptBlocks(newBucketBlocks);
        // now the bucket is ready to be written so add it to list from the back
        newPath.emplace_back(encryptedBucket);
    }
    // then write back path to tree
    oramTree.writeBackPath(pathIndices, newPath);
}

std::vector<ORAMBucket> ORAMClient::createEncryptedDummyBuckets(int blocksToStore) {
    int leavesInTree = static_cast<int>(std::pow(2 ,std::ceil(std::log2(blocksToStore))));
    int numBuckets = 2 * leavesInTree - 1;
    std::vector<ORAMBlock> dummyBlocks;
    dummyBlocks.resize(ORAMBucket::getCapacity(), ORAMBlock(maxDocIDs));
    std::vector<ORAMBucket> initBuckets(numBuckets);

    // calculate how many buckets there are in the tree and create numBuckets amount of dummy buckets to initialize tree
    // with
    for(int i = 0; i < numBuckets; i++) {
        std::vector<std::vector<unsigned char> > encryptedBucket = serializeAndEncryptBlocks(dummyBlocks);
        initBuckets[i] = ORAMBucket(encryptedBucket);
    }
    return initBuckets;
}

