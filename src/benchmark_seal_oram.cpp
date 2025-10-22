#include "../include/seal_oram.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <fstream>

void benchmarkSEAL_ORAM() {
    // 2^20 blocks
    constexpr int N = 1 << 20;
    // set max docs for each block
    int maxDocs = 20;
    // set number of accesses
    int numAccesses = 1000;

    // create a vector for the different alpha and padding values
    std::vector<int> alphas = {2, 4, 6};
    std::vector<int> paddings = {1, 2, 4};
    // test document ids to write
    std::vector<int> testDocIDs = {111, 222, 333, 444};

    // write information to a csv file
    std::ofstream out("seal_oram_benchmark.csv");
    out << "alpha,padding,time_ms\n";

    std::srand(std::time(nullptr));

    // loop over all combinations of alpha and padding values
    for (int alpha : alphas) {
        for (int x : paddings) {
            std::cout << "\n--- Running SEAL ORAM Benchmark: alpha=" << alpha << ", x=" << x << " ---\n";

            SEAL_ORAM sealOram(N, maxDocs, alpha, x);

            // start clock
            auto start = std::chrono::high_resolution_clock::now();
            // conduct accesses
            for (int i = 0; i < numAccesses; ++i) {
                // random block id
                int blockID = std::rand() % N;
                // do write and read
                sealOram.accessBlock(blockID, true, testDocIDs);
                sealOram.accessBlock(blockID, false, {});
            }

            // end timer
            auto end = std::chrono::high_resolution_clock::now();
            // calculate duration
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

            // output time
            std::cout << "alpha=" << alpha << ", x=" << x << " | Time: " << duration << " ms\n";
            // also to csv file
            out << alpha << "," << x << "," << duration << "\n";
        }
    }

    // close file
    out.close();
    // signal that tests are done
    std::cout << "\nBenchmark completed. Results saved to 'seal_oram_benchmark.csv'\n";
}

// run benchmark
int main() {
    benchmarkSEAL_ORAM();
    return 0;
}
