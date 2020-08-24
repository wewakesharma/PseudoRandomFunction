//
// Created by Vivek Sharma on 8/24/20.
//

#ifndef DARKMATTERCODE_PACKED_PRF_CENTRAL_H
#define DARKMATTERCODE_PACKED_PRF_CENTRAL_H

#include "packedMod2.hpp" // defines PackedZ2
#include "packedMod3.hpp" // defines PackedPairZ2 and PackedZ3
#include "Toeplitz-by-x.hpp" //defines N_COLS

#define N_SIZE 256

void PRF_packed_test(std::vector<uint64_t>& K1, PackedZ2<N_COLS>& x1, std::vector<uint64_t>& K2,
                          PackedZ2<N_COLS>& x2, std::vector< PackedZ3<81> >& Rmat, PackedZ3<81>& out1Z3,
                          PackedZ3<81>& out2Z3, int i);
void PRF_packed(int nTimes,  int nRuns, int nStages);
void display_time_p1();
void display_time_p3();
#endif //DARKMATTERCODE_PACKED_PRF_CENTRAL_H
