//
// Created by Vivek Sharma on 9/16/20.
//
#include <iostream>
#include "packedMod2.hpp" // defines PackedZ2
#include "packedMod3.hpp" // defines PackedPairZ2 and PackedZ3
#include "Toeplitz-by-x.hpp" //defines N_COLS

#define N_SIZE 256

#ifndef NEWPROTOCOL_NEWPROTOCOL_TEST_H
#define NEWPROTOCOL_NEWPROTOCOL_TEST_H

void newprotocol_test(std::vector<uint64_t>& K1, PackedZ2<N_COLS>& x1, std::vector<uint64_t>& K2,
                      PackedZ2<N_COLS>& x2, std::vector< PackedZ3<81> >& Rmat, PackedZ3<81>& y1_z3,
                      PackedZ3<81>& y2_z3, int nTimes);

void test_round1(PackedZ2<N_COLS>& X, PackedZ2<N_COLS>& rx, PackedZ2<N_COLS>& x_mask);
void test_round2(std::vector<uint64_t>& K, PackedZ2<N_COLS>& X,PackedZ2<N_COLS>& rw, PackedZ2<N_COLS>& w_mask);
void test_round3(std::vector<uint64_t>& K, PackedZ2<N_COLS>& X,PackedZ3<N_SIZE>& res1, PackedZ3<N_SIZE>& res2);

#endif //NEWPROTOCOL_NEWPROTOCOL_TEST_H
