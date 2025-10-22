#ifndef PATHORAM_SEAL_ORAM_H
#define PATHORAM_SEAL_ORAM_H

#include "oram_client.h"
#include <vector>
#include <cmath>
#include <unordered_map>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/sha.h>


// create seal oram interface to store alpha, x, maxDoc count, and number of ORAMS
class SEAL_ORAM {
private:
    int alpha;
    int x;
    int maxDocs;
    int numORAMS;

    // store oram instances as well
    std::vector<ORAMClient> oramInstances;
    // as well as a map to map a block to an oram and see where it belongs
    std::unordered_map<int, int> blockToORAM;

    // as well as a prp key for the prp
    unsigned char prpKey[16];

    // create function to return padding size given the real size (determined by x)
    int getPaddingSize(int realSize);
    // given a block instance return the oram instance (alpha MSBs of prp(blockID)) and phi, the location of said
    // block in the oram instance (the remaining bytes)
    std::pair <uint32_t , uint32_t> selectORAMInstanceAndPhi(int blockID);
    // given a docList pad it to the proper amount before inserting
    std::vector<int> padDocList(const std::vector<int>& docList);
    // a prp function
    uint32_t prp(int blockID);

public:
    // a constructor for the seal oram
    SEAL_ORAM(int totalBlocks, int maxDocs, int alpha, int x);

    // an access function to recreate the accessing a block in a SEAL ORAM
    std::vector<int> accessBlock(int blockID, bool isWrite, const std::vector<int>& docIDs);


};


#endif //PATHORAM_SEAL_ORAM_H
