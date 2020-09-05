//
// Created by Vivek Sharma on 9/2/20.
//

#include "newprotocol.h"
#include <cassert>
#include "packedMod2.hpp"
#include "Toeplitz-by-x.hpp"
#include "OT.hpp"
#include "mains.hpp"
#include <typeinfo>//to determine the type of variables
#include "Timing.hpp"
#include <chrono>

// in Toeplitz-by-x.hpp
// #define N_ROWS 256
// #define N_COLS 256

using namespace std;

long timerPRF = 0;
long timer_phase3 = 0;

// A place to store the results from pre-processing (GLOBAL)
vector<uint64_t> K1(toeplitzWords), K2(toeplitzWords);
PackedZ2<N_COLS> x1, x2;
static std::vector< std::vector<uint64_t> > rk1, rk2, rk;
static std::vector< PackedZ2<N_COLS> > rx1,rx2, rx;
static std::vector< PackedZ2<N_COLS> > preproc_z;


/*
 * This function generates the variables and perform computation to simulate centralized
 * version of the new protocol
 */
void PRF_new_protocol_central()
{

    //Perform preprocessing
    preProc_Toeplitz_by_x(1);//it was nRuns * 2 in previous protocol, needs to be changed later


    //generate input X and K
    randomWord(1); // use seed=1

    for (auto &w : K1) w = randomWord();
    K1[K1.size() - 1] &= topelitzMask; // turn off extra bits at the end

    for (auto &w : K2) w = randomWord();
    K2[K2.size() - 1] &= topelitzMask; // turn off extra bits at the end

    x1.randomize();
    x2.randomize();

    //generate masks rk1, rk2, rx1, rx2, and compute preproc_z = rk*rx+rw



    //Both parties locally compute [x'] = [x] + [rx] and [K'] = [K] + [rk]

    //Both parties compute w' = [k'x'] - k'[rx] - rk.x' + [rk * rk + rw]
}