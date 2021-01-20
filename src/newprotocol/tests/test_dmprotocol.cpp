//
// Created by Vivek Sharma on 9/16/20.
//
#include <cassert>
#include "packedMod2.hpp"
#include "Toeplitz-by-x.hpp"
#include "OT.hpp"
#include "mains.hpp"
#include <typeinfo>//to determine the type of variables
#include "Timing.hpp"
#include <chrono>
#include "PRF.hpp"

using namespace std;

#ifdef UNIT_DM  //tests the dark matter wPRF implementation by comparing it with PRF_centralized
int main()
{
    //declare the variables
    int nTimes = 1;
    std::vector<uint64_t> K1(toeplitzWords), K2(toeplitzWords);//key shares of parties
    PackedZ2<N_COLS> x1, x2; //input shares of parties
    std::vector<PackedZ3<81> > Rmat(256);
    PackedZ3<81> y1_z3, y2_z3,y_out_z3,out_z3;//output shares of both the parties

    //generate random inputs
    randomWord(1); // use seed=1
    for (auto &w : K1) w = randomWord();
    K1[K1.size() - 1] &= topelitzMask; // turn off extra bits at the end
    for (auto &w : K2) w = randomWord();
    K2[K2.size() - 1] &= topelitzMask; // turn off extra bits at the end
    x1.randomize();
    x2.randomize();

    //generate a 81 X 256 randomization matrix in Z3.
    for (auto &col : Rmat) // iterate over the columns
        col.randomize();

    //call the centralized version of the new protocol
    PRF_packed_centralized(K1,x1, K2,x2, Rmat,out_z3,1);

    //initialize using preproc values
    initGlobals();  // initialize some global variables
    preProc_Toeplitz_by_x(2); // pre-processing for two runs
    preProc_OT(1); //preprocess for OT, generate ra, rn, rx, and z
    PRF_DM(K1,x1,K2,x2,Rmat,y1_z3, y2_z3,0);
    y_out_z3 = y1_z3;
    y_out_z3.add(y2_z3);

    if(y_out_z3 == out_z3)
        std::cout<<"Distributed Dark Matter Protocol - Test Status... PASS"<<std::endl;
    else
        std::cout<<"Distributed Dark Matter Protocol - Test Status... FAIL"<<std::endl;

    return 0;
}




//call the PRF function to generate output
//call the PRF_packed_centralized_res_compare()

#endif