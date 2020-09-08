//
// Created by Vivek Sharma on 9/2/20.
//

#include "newprotocol.h"
#include <cassert>
#include "packedMod2.hpp"
#include "packedMod3.hpp"
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
static std::vector< PackedZ2<N_COLS> > rx1,rx2;
static std::vector< PackedZ2<N_COLS> > preproc_z;

//To store the mod 2 random values for Rk, rx and sw
static std::vector< std::vector<uint64_t> > rKs;
static std::vector< PackedZ2<N_COLS> > rx, sw, rw;


//Preprocessing
void preProc_mod2_dm2020(unsigned int nTimes)
{
    rKs.resize(nTimes, std::vector<uint64_t>(toeplitzWords));
    rx.resize(nTimes);
    rw.resize(nTimes);
    sw.resize(nTimes);
    //generate mod2 random values for rKs, rx and sw
    for(unsigned int i=0; i<nTimes; i++)
    {
        // fill rKs[*] with random Topellitz matrices
        for (auto& w : rKs[i]) w = randomWord();
        rKs[i][rKs[i].size()-1] &= topelitzMask;

        rx[i].randomize(); // random rx[i]'s
        sw[i].randomize(); // random sw[i]'s
        
        //compute rw = rk* rx ^ sw
        rw[i].toeplitzByVec(rKs[i], rx[i]);
        rw[i].add(sw[i]);
    }

}

/*
 * This function generates the variables and perform computation to simulate centralized
 * version of the new protocol
 */
void PRF_new_protocol_central()
{

    //Perform preprocessing
    preProc_mod2_dm2020(1);//it was nRuns * 2 in previous protocol, needs to be changed later

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