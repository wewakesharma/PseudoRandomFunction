// Copyright (C) 2020 Tzipora Halevi, MIT license - see LICENSE file
/** Toeplitz-by-x.hpp
 *  - declerations for a protocol for A*x (aka A*x+b) mod 2
 *    A is a Toeplitz matrix
 */
#include "packedMod2.hpp" // defines PackedZ2
#include "packedMod3.hpp" // defines PackedPairZ2 and PackedZ3
#include "Toeplitz-by-x.hpp" //defines N_COLS
#include "OT.hpp"

#define N_SIZE 256
#define N_COLS 256
#define N_ROWS 256

//extern variable for initGlobals
extern std::vector<uint64_t> mA_global;

//extern variable for preProc_Toeplitz_by_x
extern std::vector< std::vector<uint64_t> > rAs;
extern std::vector< PackedZ2<N_ROWS> > rbs, rzs;
extern std::vector< PackedZ2<N_COLS> > rxs;

//extern variable for preProc_OT
extern std::vector< PackedZ2<N_SIZE> > rxps;
extern std::vector< PackedPairZ2<N_SIZE> > raps, rbps, zps;

void PRF_DM_wpreproc(unsigned int nTimes,  int nRuns, int nStages);
void PRF_packed_unit_test(std::vector<uint64_t>& K1, PackedZ2<N_COLS>& x1, std::vector<uint64_t>& K2,
                          PackedZ2<N_COLS>& x2, std::vector< PackedZ3<81> >& Rmat, PackedZ3<81>& out1Z3,
                          PackedZ3<81>& out2Z3, int i);


void display_Phase3_runtime(float& time_unit_multiplier);//measure the runtime of phase 3
void display_PRF_runtime(float& time_unit_multiplier); //measure the runtime of entire PRF

void PRF_packed_centralized(std::vector<uint64_t>& K1, PackedZ2<N_COLS>& x1, std::vector<uint64_t>& K2,
                           PackedZ2<N_COLS>& x2, std::vector< PackedZ3<81> >& Rmat,  PackedZ3<81>& outZ3, int nTimes);
void PRF_DM(std::vector<uint64_t>& K1, PackedZ2<N_COLS>& x1, std::vector<uint64_t>& K2,
            PackedZ2<N_COLS>& x2, std::vector< PackedZ3<81> >& Rmat, PackedZ3<81>& out1Z3,
            PackedZ3<81>& out2Z3, int nRuns, int nTimes);
