//
// Created by Vivek Sharma on 9/16/20.
//

#include <iostream>
#include <cstdlib>
#include "packedMod2.hpp"
#include "packedMod3.hpp"
#include "mains.hpp"
//#include "PRF.hpp"
#include "OT.hpp"
#include "Timing.hpp"
#include "newprotocol.h"
#include "newprotocol_test.h"
#include <chrono>

using namespace std;


/*
 * Main driver program to start the execution of newprotocol
 * the test code is in newprotocol_test() which is in test_newprotocol.cpp
 */
#ifdef TEST_NP
int main()
{
    //declaring the inputs
    int nTimes = 1;//keep it constant for 1 run
    std::vector<uint64_t> K1(toeplitzWords), K2(toeplitzWords);//key shares of parties
    PackedZ2<N_COLS> x1, x2; //input shares of parties
    PackedZ3<81> y_out_z3, y1_z3, y2_z3;//output of new protocol

#ifdef DEBUG
    std::cout<<"test_newprotocol/main.cpp: Protocol Execution begins"<<std::endl;
#endif

    //2. generate input X and key K
    randomWord(1); // use seed=1
    for (auto &w : K1) w = randomWord();
    K1[K1.size() - 1] &= topelitzMask; // turn off extra bits at the end
    for (auto &w : K2) w = randomWord();
    K2[K2.size() - 1] &= topelitzMask; // turn off extra bits at the end
    x1.randomize();
    x2.randomize();

    //generate a 81 X 256 randomization matrix in Z3.
    std::vector<PackedZ3<81> > Rmat(256);
    for (auto &col : Rmat) // iterate over the columns
        col.randomize();

    //call the centralized version of the new protocol
    PRF_new_protocol_central(K1,x1,K2, x2, Rmat, y1_z3, y2_z3, nTimes);

#ifdef DEBUG
    std::cout<<"test_newprotocol/main.cpp: Protocol Execution ends"<<std::endl;
    std::cout<<"test_newprotocol/main.cpp: Calling the newprotocol test function"<<std::endl;
#endif

    newprotocol_test(K1,x1,K2,x2,Rmat,y1_z3, y2_z3, nTimes);
    return 0;
}
#endif
