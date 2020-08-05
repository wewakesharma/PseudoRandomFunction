// Copyright (C) 2020 Tzipora Halevi, MIT license - see LICENSE file
/** Toeplitz-by-x.hpp
 *  - declerations for a protocol for A*x (aka A*x+b) mod 2
 *    A is a Toeplitz matrix
 */
#include "packedMod2.hpp"

#define N_ROWS 256 
#define N_COLS 256
#define N_SIZE 256

extern uint64_t topelitzMask; // mask to remove extra bits in the rAs
extern const unsigned int toeplitzWords;

// poor-man's implementation of communication channels
void initGlobals(PackedZ2<N_ROWS>& b, const std::vector<uint64_t>& A);

// A "trusted party implementation" of pre-processing
void preProc_Toeplitz_by_x(unsigned int nTimes);


void p1_recv();

void topelitz_Party1(PackedZ2<N_ROWS>& out, const std::vector<uint64_t>& A,
                     int index);

void topelitz_Party2_1(PackedZ2<N_COLS>& x, int index);
void topelitz_Party2_2(PackedZ2<N_ROWS>& out,PackedZ2<N_COLS>& x,int index);
