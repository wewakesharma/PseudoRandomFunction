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

void PRF_new_protocol(std::vector<uint64_t>& K1, PackedZ2<N_COLS>& x1,
                              std::vector<uint64_t>& K2, PackedZ2<N_COLS>& x2, std::vector<PackedZ3<81> >& Rmat,
                              PackedZ3<81>& y1_z3, PackedZ3<81>& y2_z3, unsigned int nRuns);

void preProc_mod3_dm2020(unsigned int nTimes);
void preProc_mod2_dm2020(unsigned int nTimes);
void fetchPreproc_party1(PackedZ2<N_COLS>& rx1, PackedZ2<N_COLS>& rw1, PackedZ2<N_COLS>& sw1,
                         std::vector<uint64_t>& rK1, PackedZ3<N_SIZE>& r0z1, PackedZ3<N_SIZE>& r1z1);
void fetchPreproc_party2(PackedZ2<N_COLS>& rx2, PackedZ2<N_COLS>& rw2, PackedZ2<N_COLS>& sw2,
                         std::vector<uint64_t>& rK2, PackedZ3<N_SIZE>& r0z2, PackedZ3<N_SIZE>& r1z2);
void party1_round_1(PackedZ2<N_COLS>& x1_mask, std::vector<uint64_t>& K1_mask,
                    PackedZ2<N_COLS>& x1, PackedZ2<N_COLS>& rx1, std::vector<uint64_t>& K1, std::vector<uint64_t>& rK1);
void party2_round_1(PackedZ2<N_COLS>& x2_mask, std::vector<uint64_t>& K2_mask,
                    PackedZ2<N_COLS>& x2, PackedZ2<N_COLS>& rx2, std::vector<uint64_t>& K2, std::vector<uint64_t>& rK2);
void compute_input_mask(PackedZ2<N_COLS>& x_mask, std::vector<uint64_t>& K_mask,PackedZ2<N_COLS>& x1_mask,
                        PackedZ2<N_COLS>& x2_mask,std::vector<uint64_t>& K1_mask,std::vector<uint64_t>& K2_mask);
void party1_round2(PackedZ2<N_COLS>& w1_mask, std::vector<uint64_t>& K_mask, PackedZ2<N_COLS>& x_mask, PackedZ2<N_COLS>& rx1,
                   std::vector<uint64_t>& rK1, PackedZ2<N_COLS>& x1, PackedZ2<N_COLS>& sw1);
void party2_round2(PackedZ2<N_COLS>& w2_mask, std::vector<uint64_t>& K_mask,PackedZ2<N_COLS>& x_mask, PackedZ2<N_COLS>& rx2,
                   std::vector<uint64_t>& rK2, PackedZ2<N_COLS>& x2, PackedZ2<N_COLS>& sw2);
void compute_wmask(PackedZ2<N_COLS>& w_mask, PackedZ2<N_COLS>& w1_mask, PackedZ2<N_COLS>& w2_mask);
PackedZ2<N_COLS> fetch_rw_global();

#endif //NEWPROTOCOL_NEWPROTOCOL_H
