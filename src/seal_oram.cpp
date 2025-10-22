#include "../include/seal_oram.h"
#include <iostream>

SEAL_ORAM::SEAL_ORAM(int totalBlocks, int maxDocs, int alpha, int x)
    : alpha(alpha), x(x), maxDocs(maxDocs) {
    int totalBlocksPadded = x * maxDocs;
    numORAMS = std::pow(2, alpha);
    int blocksPerORAM = totalBlocksPadded / numORAMS;


    for(int i = 0; i < numORAMS; i++) {
        oramInstances.emplace_back(blocksPerORAM, maxDocs);
    }

    RAND_bytes(prpKey, sizeof(prpKey));
}

uint32_t SEAL_ORAM::prp(int blockID) {
    // Create buffer for input
    unsigned char input[sizeof(blockID)];
    std::memcpy(input, &blockID, sizeof(blockID));

    // Hash output
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(input, sizeof(input), hash);

    // Extract 4 bytes from the hash to get a uint32_t PRP value
    uint32_t prpVal;
    std::memcpy(&prpVal, hash, sizeof(prpVal));

    return prpVal;
}

std::pair<uint32_t , uint32_t> SEAL_ORAM::selectORAMInstanceAndPhi(int blockID) {
    // if we cant find mapped instance
    if (blockToORAM.find(blockID) == blockToORAM.end()) {
        // get permuted ID
        uint32_t permutedID = prp(blockID);
        // select first alpha bits of it
        uint32_t oramIndex = permutedID >> (32 - alpha);
        uint32_t phi = permutedID & ((1 << (32-alpha)) - 1);


        // update mapping
        blockToORAM[blockID] = oramIndex;
        return std::pair(blockToORAM[blockID], phi);
    }
    return std::pair(blockToORAM[blockID], -1);
}

int SEAL_ORAM::getPaddingSize(int realSize) {
    int i = 0;

    while(!(realSize <= std::pow(x, i) && std::pow(x, i-1) < realSize)) {
        i++;
    }
    return static_cast<int>(std::pow(x, i));
}

std::vector<int> SEAL_ORAM::padDocList(const std::vector<int> &docList) {
    int paddedSize = getPaddingSize(docList.size());
    std::vector<int> paddedDocs = docList;

    while(paddedDocs.size() < paddedSize) {
        paddedDocs.push_back(-1);
    }
    return paddedDocs;
}

std::vector<int> SEAL_ORAM::accessBlock(int blockID, bool isWrite, const std::vector<int> &docIDs) {
    auto [oramIndex, phi] = selectORAMInstanceAndPhi(blockID);
    std::vector<int> paddedDocIDs = (isWrite) ? padDocList(docIDs) : std::vector<int>();
    std::vector<int> retrieved = oramInstances[oramIndex].accessBlock(phi, isWrite, paddedDocIDs);

    return retrieved;
}
