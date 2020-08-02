// Copyright (C) 2020 Tzipora Halevi, MIT license - see LICENSE file
/** test_Toeplitz-by-x.cpp
 *  - testing the protocol for A*x (aka A*x+b) mod 2
 *    A is a Toeplitz matrix
 */
#include "packedMod2.hpp"
#include "Toeplitz-by-x.hpp"
#include "mains.h"

// poor-man's implementation of communication channels
void initGlobals(PackedZ2<N_ROWS>& b, const std::vector<uint64_t>& A);

// A "trusted party implementation" of pre-processing
void preProc_Toeplitz_by_x(unsigned int nTimes);

void topelitz_Party1(const std::vector<uint64_t>& A,
                     PackedZ2<N_ROWS>& b, int index);

void topelitz_Party2(PackedZ2<N_ROWS>& y, PackedZ2<N_COLS>& x, int index);

// This test program implements the first phase of the PRF
int main()
{
    randomWord(1); // use seed=1
    std::vector<uint64_t> K1(toeplitzWords), K2(toeplitzWords);
    PackedZ2<N_COLS> x1, x2;

    PackedZ2<N_ROWS> out1_A, out2_A, out1_B, out2_B;

    initGlobals(out1_A, K1);  // initialize some global variables
    preProc_Toeplitz_by_x(2); // pre-processing for two runs
 
    // Choose random K1, K2, x1, x2, we will be computing
    // (K1 xor K2) \times (x1 xor x2)

    for (auto& w : K1) w = randomWord();
    K1[K1.size()-1] &= topelitzMask; // turn off extra bits at the end
    
    for (auto& w : K2) w = randomWord();
    K2[K2.size()-1] &= topelitzMask; // turn off extra bits at the end
    
    x1.randomize();
    x2.randomize();

    // First run, a protocol for K1 times x2
    topelitz_Party2_1(x2, 0);
    topelitz_Party1(out1_A, K1, 0);
    topelitz_Party2_2(out1_B, x2, 0);

    // Second run, a protocol for K2 times x1
    topelitz_Party2_1(x1, 0);
    topelitz_Party1(out2_B, K2, 0); // same FIXME as above
    topelitz_Party2_2(out2_A, x1, 0);

    // Party1 computes locally K1 times x1, and adds to out1_A,out2_A
    out1_A.add(out2_A);           // out1 ^= out2
    out2_A.toeplitzByVec(K1, x1); // K1 times x1
    out1_A.add(out2_A);           // sum of all terms

    // Party2 computes locally K2 times x2, and adds to out1_B,out2_N
    out1_B.add(out2_B);           // out1 ^= out2
    out2_B.toeplitzByVec(K2, x2); // K2 times x2
    out1_B.add(out2_B);           // sum of all terms

    // check the result

    out1_B.add(out1_A);                  // add the two shares

    x1.add(x2);                          // set x = x1 ^ x2
    for (unsigned i=0; i<K1.size(); i++) // Set K = K1 ^ K2
        K1[i] ^= K2[i];
    
    out1_A.toeplitzByVec(K1, x1);        // re-compute K*x locally    

    if (out1_A == out1_B)
        std::cerr << "Yay, mult-by-topelit succeeded"<<std::endl;
    else
        std::cerr << "Boo, mult-by-topelit failed"<<std::endl;
}
