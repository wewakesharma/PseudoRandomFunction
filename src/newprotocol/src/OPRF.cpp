//
// Created by Vivek Sharma on 11/30/20.
//
#include <iostream>
#include "newprotocol.hpp"
#include <cassert>
#include "packedMod2.hpp"
#include "packedMod3.hpp"
#include "Toeplitz-by-x.hpp"
#include "OT.hpp"
#include "mains.hpp"
#include <typeinfo>//to determine the type of variables
#include "Timing.hpp"
#include <chrono>
#include "packed_PRF_central.h"
#include "newprotocol_test.hpp"
#include "PRF.hpp"
#include "lookup_functions.h"
#include "OPRF.h"

void preproc_TrustedParty()
{
    //generate rK, rq, rx
    std::vector<uint64_t> rK(toeplitzWords);
    PackedZ2<N_COLS> rq, rx, v;

    for (auto &w : rK) w = randomWord();
    rK[rK.size() - 1] &= topelitzMask; // turn off extra bits at the end
    rq.randomize();
    rx.randomize();

    v.toeplitzByVec(rK,rx);
    v.add(rq);

}

void oblivious_PRF(std::vector<uint64_t>& K, PackedZ2<N_COLS>& x, std::vector<PackedZ3<81> >& Rmat,
                   PackedZ3<81>& y_out_z3, unsigned int nRuns)
{
    //preprocess the input
    preproc_TrustedParty();
    //server_step1
    //client_step1
    //server_step2
    //client_step2
    //server_step3
    //client_step3
}