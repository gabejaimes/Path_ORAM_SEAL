#ifndef PATHORAM_ORAM_BUCKET_H
#define PATHORAM_ORAM_BUCKET_H

#include "oram_block.h"
#include <vector>
#include "aes256.h"

// create ORAM bucket to hold vector of unsigned chars (encrypted blocks) and store size of bucket
class ORAMBucket {
private:
    std::vector<std::vector<unsigned char> > encryptedBlocks;
    static const size_t bucketCapacity = 4;

public:
    // Dummy Constructor
    ORAMBucket();

    // Constructor with Blocks
    explicit ORAMBucket(std::vector<std::vector<unsigned char> > bucketBlocks);

    // Getters
    [[nodiscard]] std::vector<std::vector<unsigned char> > getEncryptedBlocks() const;
    [[nodiscard]] static size_t getCapacity() ;

    // Setters
    void setBlocks(const std::vector<std::vector<unsigned char> >& newBlocks);

};
#endif //PATHORAM_ORAM_BUCKET_H
