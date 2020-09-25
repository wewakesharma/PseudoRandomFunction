//
// Created by Vivek Sharma on 9/2/20.
//
#include <iostream>
#include <cstdlib>
#include "packedMod2.hpp"
#include "packedMod3.hpp"
#include "mains.hpp"
#include "PRF.hpp"
#include "OT.hpp"
#include "Timing.hpp"
#include "newprotocol.h"
#include "newprotocol_test.h"
#include <chrono>

using namespace std;


/*
void PRF_packed_centralized_res_compare(std::vector<uint64_t>& K1, PackedZ2<N_COLS>& x1, std::vector<uint64_t>& K2,
                                        PackedZ2<N_COLS>& x2, std::vector< PackedZ3<81> >& Rmat, PackedZ3<81>& y1_z3,
                                        PackedZ3<81>& y2_z3, int nTimes)
{

    PackedZ3<81> outZ3;//final Z3 outpu;

    PRF_packed_centralized(K1,  x1,  K2,
                           x2,  Rmat, outZ3, nTimes);

    PackedZ3<81>out_12_Z3 = y1_z3;
    out_12_Z3.add(y2_z3);//merged output from parameters

    //  out_12_Z3.compute_y_out(out1Z3, out2Z3);

#ifdef DEBUG
    std::cout<<"newprotocol.cpp/PRF_new_protocol_central(): Output of out_12_Z3"<<std::endl;
    for(int c = 0; c<81;c++)
    {
        std::cout<<out_12_Z3.at(c);
    }
    std::cout<<""<<std::endl;
    std::cout<<"newprotocol.cpp/PRF_new_protocol_central(): Output of out_Z3"<<std::endl;
    for(int c = 0; c<81;c++)
    {
        std::cout<<outZ3.at(c);
    }
    std::cout<<""<<std::endl;
#endif

    if(out_12_Z3 == outZ3)
        cout<<endl<<"PRF packed test: Test passed";
    else
        cout<<endl<<"PRF packed test: Test fails";
}*/

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

    #ifdef DEBUG//K1, K2 , x1 and x2 are set to 1 for debugging purpose
        K1= {1,0,0,0,0,0,0,0};
        K2 = {0,0,0,0,0,0,0,0};

        x1.reset();
        x2.reset();
     //   x1.set(0,1);
       // x2.set(1,1);

    #endif

    //generate a 81 X 256 randomization matrix in Z3.
    std::vector<PackedZ3<81> > Rmat(256);
    for (auto &col : Rmat) // iterate over the columns
        col.randomize();

    #ifdef DEBUG //Rmat first value has been set to one(1)
        //setting Rmat as 1
        for (int i = 0; i < 256; i++)
        {
            Rmat[i].reset();
        }
        Rmat[0].set(0,1);
        //std::cout<<"Rmat "<<Rmat<<std::endl;
    #endif

    /*
     * Centralized implementation of the PRF ((K*x) * Rmat)
     */
    PackedZ3<81> outZ3Central;//final centralized PRF Z3 output;
    PRF_packed_centralized(K1,  x1,  K2,
                           x2,  Rmat, outZ3Central, nTimes);

    //calling the distributed version of the new PRF protocol
    PRF_new_protocol(K1,x1,K2, x2, Rmat, y1_z3, y2_z3, nTimes);

#ifdef DEBUG
    std::cout<<"test_newprotocol/main.cpp: Protocol Execution ends"<<std::endl;
    std::cout<<"test_newprotocol/main.cpp: Calling the newprotocol test function"<<std::endl;
#endif





//    PRF_packed_centralized_res_compare(K1,x1,K2,x2,Rmat,y1_z3,y2_z3,nTimes);

    PackedZ3<81>out_dist_Z3 = y1_z3;
    out_dist_Z3.add(y2_z3);//merged output from parameters

    std::cout<<"out_dist_Z3 "<<out_dist_Z3<<std::endl;
    std::cout<<"outZ3Central "<<outZ3Central<<std::endl;
    if(out_dist_Z3 == outZ3Central)
        cout<<endl<<"PRF packed test: Test passed";
    else
        cout<<endl<<"PRF packed test: Test fails";

    return 0;
}
#endif

/*
 * newprotocol_test - tests
 */
