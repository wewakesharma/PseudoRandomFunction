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

#ifdef TEST_OPRF

void centralPRF(std::vector<uint64_t>& K, PackedZ2<N_COLS>& x, std::vector<PackedZ3<81> >& Rmat,
                PackedZ3<81>& outZ3)
{
    /* centralized implementation
     * BEGIN
     */
    PackedZ2<N_COLS> outKX;
    outKX.toeplitzByVec(K,x);
    std::vector<unsigned int> outKX_unsgn;//unsigned int of outKX i.e.(K*x)
    outKX.toArray(outKX_unsgn);

    PackedZ3<256> outKX_Z3;//packed Z3
    //PackedZ3<81> outZ3;//final Z3 output
    outKX_Z3.fromArray(outKX_unsgn);//converting unsigned int to PackedZ2
    outZ3.matByVec(Rmat,outKX_Z3);//output of randmat*K*x
}

int main()
{
    //declare the input
    int nRuns = 1000;       //number of runs
    std::vector<uint64_t> K(toeplitzWords);//key shares of parties
    PackedZ2<N_COLS> x; //input shares of parties
    PackedZ3<81> y_out_z3, y1_z3, y2_z3, outZ3;//output of new protocol

    //generate the input
    randomWord(1); // use seed=1
    for (auto &w : K) w = randomWord();
    K[K.size() - 1] &= topelitzMask; // turn off extra bits at the end

    x.randomize();

#ifdef DEBUG
    K={0,0,0,0,0,0,0,0};
    x.reset();
    x.set(0,0);
#endif

    //generate a 81 X 256 randomization matrix in Z3.
    std::vector<PackedZ3<81> > Rmat(256);
    for (auto &col : Rmat) // iterate over the columns
        col.randomize();

#ifdef DEBUG
    for (int i = 0; i < 256; i++)
    {
        Rmat[i].reset();
    }
    Rmat[0].set(0,1);
#endif

#ifdef OPRF_PRINT_VAL
    std::cout<<"test_oprf.cpp/main(): printing the input values "<<std::endl;
    std::cout<<"x: "<<x<<std::endl;
    std::cout<<"K: ";
    for(int i =0; i< K.size();i++)
    {
        std::cout<<K[i]<<" \t";
    }
    std::cout<<std::endl;
    //std::cout<<"Rmat "<<Rmat<<std::endl;
    std::cout<<std::endl;
#endif

    oblivious_PRF(K,x,Rmat,y_out_z3,nRuns);   //driver code that will initiate the protocol.

    centralPRF(K,x,Rmat,outZ3); //centralized implementation for testing

#ifdef OPRF_PRINT_VAL
    std::cout<<"Output of centralized PRF "<<outZ3<<std::endl;
    std::cout<<"The output of 23-OPRF is "<<y_out_z3<<std::endl;
    if (outZ3==y_out_z3)
        std::cout << "in test_oprf.cpp, main, test PASSES. "<<std::endl;
    else
        std::cout << "in test_oprf.cpp, main, test FAILS. "<<std::endl;
#endif
    return 0;
}

#endif