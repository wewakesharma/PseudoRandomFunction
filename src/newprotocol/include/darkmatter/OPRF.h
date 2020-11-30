//
// Created by Vivek Sharma on 11/30/20.
//
#include <iostream>
#include "packedMod2.hpp" // defines PackedZ2
#include "packedMod3.hpp" // defines PackedPairZ2 and PackedZ3
#include "Toeplitz-by-x.hpp" //defines N_COLS

#define N_SIZE 256

#ifndef NEWPROTOCOL_OPRF_H
#define NEWPROTOCOL_OPRF_H

void oblivious_PRF(std::vector<uint64_t>& K, PackedZ2<N_COLS>& x, std::vector<PackedZ3<81> >& Rmat,
                   PackedZ3<81>& y_out_z3, unsigned int nRuns);

#endif //NEWPROTOCOL_OPRF_H
