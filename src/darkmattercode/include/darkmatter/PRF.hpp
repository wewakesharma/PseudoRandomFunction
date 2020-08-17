// Copyright (C) 2020 Tzipora Halevi, MIT license - see LICENSE file
/** Toeplitz-by-x.hpp
 *  - declerations for a protocol for A*x (aka A*x+b) mod 2
 *    A is a Toeplitz matrix
 */
#include "packedMod2.hpp" // defines PackedZ2
#include "packedMod3.hpp" // defines PackedPairZ2 and PackedZ3
#include "Toeplitz-by-x.hpp" //defines N_COLS

#define N_SIZE 256

void PRF_DM(unsigned int nTimes,  int nRuns, int nStages);
void PRF_packed_unit_test(std::vector<uint64_t>& K1, PackedZ2<N_COLS>& x1, std::vector<uint64_t>& K2,
                          PackedZ2<N_COLS>& x2, std::vector< PackedZ3<81> >& Rmat, PackedZ3<81>& out1Z3,
                          PackedZ3<81>& out2Z3, int i);
void PRF_unpacked_test();

void display_Phase3_runtime();//measure the runtime of phase 3
void display_PRF_runtime(); //measure the runtime of entire PRF