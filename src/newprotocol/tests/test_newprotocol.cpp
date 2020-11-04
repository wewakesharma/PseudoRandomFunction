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
#include "newprotocol.hpp"
#include "newprotocol_test.hpp"
#include <chrono>
#include "lookup_functions.h"

using namespace std;

/*
void test_round2_unit(std::vector<uint64_t>& K1, PackedZ2<N_COLS>& x1,
                      std::vector<uint64_t>& K2, PackedZ2<N_COLS>& x2,
                      int nTimes)
{
    std::vector<uint64_t> rK1(toeplitzWords), rK2(toeplitzWords), rK(toeplitzWords);
    PackedZ2<N_COLS> rx1, rx2, rx; //mask for input
    PackedZ2<N_COLS> sw1, sw2, sw;
    PackedZ2<N_COLS> rw1, rw2, rw;//rw = rK * rx + sw

    //declare a variable in Z3 called r0z and r1z
    PackedZ3<N_SIZE> r0z, r1z;
    PackedZ3<N_SIZE> r0z1, r0z2, r1z1, r1z2;

    PackedZ3<81> y_out_z3;//output of each parties

    //============================

    //1. Perform preprocessing
    preProc_mod2_dm2020(nTimes);//it was nRuns * 2 in previous protocol, needs to be changed later
    preProc_mod3_dm2020(nTimes);

    fetchPreproc_party1(rx1,rw1,sw1,rK1,r0z1,r1z1);
    fetchPreproc_party2(rx2,rw2,sw2,rK2, r0z2,r1z2);

#ifdef PRINT_VAL
    std::cout<<"in PRF_new_protocol, rx1= "<<rx1<<std::endl;
    std::cout<<"PRF_new_protocol, rx2 "<<rx2<<std::endl;
#endif

    //3. Parties locally compute [x'] = [x] + [rx] and [K'] = [K] + [rk]
    PackedZ2<N_COLS> x1_mask, x_mask, x2_mask;
    std::vector<uint64_t> K1_mask(toeplitzWords), K2_mask(toeplitzWords), K_mask(toeplitzWords);

    party1_round_1(x1_mask,K1_mask,x1,rx1,K1,rK1);
    party2_round_1(x2_mask,K2_mask,x2,rx2,K2,rK2);

    //std::cout<<""<<std::endl;
    //both the parties are supposed to exchange their mask values
    compute_input_mask(x_mask, K_mask, x1_mask, x2_mask, K1_mask, K2_mask);

#ifdef PRINT_VAL
    std::cout<<"newprotocol.cpp/PRF_new_protocol_central(): Round 1 ends"<<std::endl;
#endif

#ifdef PRINT_VAL
    std::cout<<"newprotocol.cpp/PRF_new_protocol_central(): Round 2 begins"<<std::endl;
#endif

    //Round 2: Both parties compute w' = [k'x'] - k'[rx] - rk.x' + [rk * rk + rw]
    PackedZ2<N_COLS> w1_mask, w2_mask, w_mask; //w' = K'(x' - rx) - rK'*x' + sw
    party1_round2(w1_mask, K_mask,x_mask, rx1, rK1, x1, sw1);
    party2_round2(w2_mask, K_mask,x_mask, rx2, rK2, x2, sw2);

    compute_wmask(w_mask, w1_mask, w2_mask);

    PackedZ2<N_COLS> X = x1; //declare a variable
    X ^= x2;    //x = x1 + x2
    std::vector<uint64_t> K(toeplitzWords);
    for (int i = 0; i < K.size(); i++)
    {
        K[i] = K1[i] ^ K2[i];
    }

    rw=fetch_rw_global ();

    test_round2(K, X, rw, w_mask);
}*/

/*
 * Main driver program to start the execution of newprotocol
 * the test code is in newprotocol_test() which is in test_newprotocol.cpp
 */




#ifdef TEST_NP
int main()
{
    //declaring the inputs
    int nRuns = 1000;
    std::vector<uint64_t> K1(toeplitzWords), K2(toeplitzWords);//key shares of parties
    PackedZ2<N_COLS> x1, x2; //input shares of parties
    PackedZ3<81> y_out_z3, y1_z3, y2_z3;//output of new protocol

#ifdef PRINT_VAL
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
        x1.set(0,1);
   //     x2.set(1,1);
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
        Rmat[0].set(0,2);
        Rmat[0].set(1,2);
        //std::cout<<"Rmat "<<Rmat<<std::endl;
    #endif


     //Centralized implementation of the PRF ((K*x) * Rmat)
    PackedZ3<81> outZ3Central;//final centralized PRF Z3 output;
    PRF_packed_centralized(K1,  x1,  K2,
                           x2,  Rmat, outZ3Central, nRuns);




#ifdef UNITTEST_ROUND2
    test_round2_unit(K1,x1,K2, x2,nTimes);
#endif
    std::cout<<"Executing new protocol"<<std::endl;
    //calling the distributed version of the new PRF protocol
    PRF_new_protocol(K1,x1,K2, x2, Rmat, y1_z3, y2_z3, nRuns);

#ifdef PRINT_VAL
    std::cout<<"test_newprotocol/main.cpp: Protocol Execution ends"<<std::endl;
    std::cout<<"test_newprotocol/main.cpp: Calling the newprotocol test function \n"<<std::endl;
#endif

/*
 * Code to compare the output of New protocol with the centralized implementation
 */
/*
    PackedZ3<81>out_dist_Z3 = y1_z3;
    out_dist_Z3.add(y2_z3);//merged output from parameters

#ifdef PRINT_VAL
    std::cout<<"out_dist_Z3 "<<out_dist_Z3<<std::endl;
    std::cout<<"outZ3Central "<<outZ3Central<<std::endl;
#endif

    if(out_dist_Z3 == outZ3Central)
        std::cout<<std::endl<<"PRF packed test: Test passed"<<std::endl;
    else
        std::cout<<std::endl<<"PRF packed test: Test fails"<<std::endl;
*/

    //===============================LOOKUP IMPLEMENTATION=================
#ifdef LOOKUP //enable in main.hpp to compute using lookup table
independent_lookup_implementation(nRuns);
exec_lookup_timing();
#endif
    return 0;
}
#endif

