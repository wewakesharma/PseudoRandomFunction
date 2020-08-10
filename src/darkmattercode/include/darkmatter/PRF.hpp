// Copyright (C) 2020 Tzipora Halevi, MIT license - see LICENSE file
/** Toeplitz-by-x.hpp
 *  - declerations for a protocol for A*x (aka A*x+b) mod 2
 *    A is a Toeplitz matrix
 */
#include "packedMod2.hpp" // defines PackedZ2
#include "packedMod3.hpp" // defines PackedPairZ2 and PackedZ3

#define N_SIZE 256

void PRF_DM(unsigned int nTimes,  int nRuns, int nStages);