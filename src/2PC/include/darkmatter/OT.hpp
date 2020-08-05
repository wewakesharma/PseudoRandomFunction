// Copyright (C) 2020 Tzipora Halevi, MIT license - see LICENSE file
/** Toeplitz-by-x.hpp
 *  - declerations for a protocol for A*x (aka A*x+b) mod 2
 *    A is a Toeplitz matrix
 */
//#include "packedMod2.hpp"

#define N_SIZE 256

typedef std::pair < PackedZ2<N_SIZE>,PackedZ2<N_SIZE>  > PackedPairZ2;

inline PackedPairZ2 operator^= (PackedPairZ2& fPPZ2, PackedPairZ2& sPPZ2){
    fPPZ2.first ^= sPPZ2.first;
    fPPZ2.second ^= sPPZ2.second;
    return fPPZ2;
}

inline PackedPairZ2 randomize(PackedPairZ2& fPPZ2){
    fPPZ2.first.randomize();
    fPPZ2.second.randomize();
    return fPPZ2;
}

void preProc_OT(unsigned int nTimes);
void OT_Party1(PackedPairZ2& r0,  PackedPairZ2& r1, int index);

void OT_Party2_1(PackedZ2<N_SIZE>& x,  int index);
void OT_Party2_2(PackedZ2<N_SIZE>& x, PackedPairZ2& out, int index);

void SC_Party2_1(PackedZ2<N_SIZE>& x, int index);
void SC_Party1(PackedZ2<N_SIZE>& y1, PackedZ3<N_SIZE>& r, int index);
void SC_Party2_2( PackedZ2<N_SIZE>& y2, PackedZ3<N_SIZE>& out, int index);

