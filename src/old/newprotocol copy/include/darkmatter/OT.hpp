// Copyright (C) 2020 Tzipora Halevi, MIT license - see LICENSE file
/** Toeplitz-by-x.hpp
 *  - declerations for a protocol for A*x (aka A*x+b) mod 2
 *    A is a Toeplitz matrix
 */
#include "packedMod2.hpp" // defines PackedZ2
#include "packedMod3.hpp" // defines PackedPairZ2 and PackedZ3

#define N_SIZE 256

void preProc_OT(unsigned int nTimes);

void OT_Party1(const PackedPairZ2<N_SIZE>& r0,  const PackedPairZ2<N_SIZE>& r1, int index);
void OT_Party2_1(const PackedZ2<N_SIZE>& x,  int index);
void OT_Party2_2(const PackedZ2<N_SIZE>& x, PackedPairZ2<N_SIZE>& out, int index);

void SC_Party2_1(const PackedZ2<N_SIZE>& y2, int index);
void SC_Party1(const PackedZ2<N_SIZE>& y1, PackedZ3<N_SIZE>& r, int index);
void SC_Party2_2(const PackedZ2<N_SIZE>& y2, PackedZ3<N_SIZE>& out, int index);

void display_SC_runtime();