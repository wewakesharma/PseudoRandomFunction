//
// Created by Vivek Sharma on 11/30/20.
//

#include <iostream>
#include <cstdlib>
#include "packedMod2.hpp"
#include "packedMod3.hpp"
#include "mains.hpp"
#include "PRF.hpp"
#include "OT.hpp"
#include "Timing.hpp"
#include "newprotocol.hpp"
#include "newprotocol_test.hpp"
#include <chrono>
#include "lookup_functions.h"
#include "OPRF.h"

using namespace std;

#define TEST_OPRF

int main()
{
    //declare the input
    int nRuns = 1000;       //number of runs
    std::vector<uint64_t> K(toeplitzWords);//key shares of parties
    PackedZ2<N_COLS> x; //input shares of parties
    PackedZ3<81> y_out_z3, y1_z3, y2_z3;//output of new protocol

    //generate the input
    randomWord(1); // use seed=1
    for (auto &w : K) w = randomWord();
    K[K.size() - 1] &= topelitzMask; // turn off extra bits at the end

    x.randomize();

    //generate a 81 X 256 randomization matrix in Z3.
    std::vector<PackedZ3<81> > Rmat(256);
    for (auto &col : Rmat) // iterate over the columns
        col.randomize();

    oblivious_PRF(K,x,Rmat,y_out_z3,nRuns);   //driver code that will initiate the protocol.
    std::cout<<"The output of 23-OPRF is "<<y_out_z3;
    return 0;
}