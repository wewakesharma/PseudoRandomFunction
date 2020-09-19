//
// Created by Vivek Sharma on 9/2/20.
//

#include <iostream>
#include "packedMod2.hpp" // defines PackedZ2
#include "packedMod3.hpp" // defines PackedPairZ2 and PackedZ3
#include "Toeplitz-by-x.hpp" //defines N_COLS

#define N_SIZE 256

#ifndef NEWPROTOCOL_NEWPROTOCOL_H
#define NEWPROTOCOL_NEWPROTOCOL_H

void PRF_new_protocol_central(std::vector<uint64_t>& K1, PackedZ2<N_COLS>& x1,
                              std::vector<uint64_t>& K2, PackedZ2<N_COLS>& x2, std::vector<PackedZ3<81> >& Rmat,
                              PackedZ3<81>& y1_z3, PackedZ3<81>& y2_z3, int nTimes);
#endif //NEWPROTOCOL_NEWPROTOCOL_H
