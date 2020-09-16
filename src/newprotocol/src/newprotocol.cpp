//
// Created by Vivek Sharma on 9/2/20.
//

#include <iostream>
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
#include "packed_PRF_central.h"
#include "PRF.hpp"

// in Toeplitz-by-x.hpp
// #define N_ROWS 256
// #define N_COLS 256

//using namespace std;


// Declaring the Keys and inputs of two parties (GLOBAL)


//Declaring shares of key and input masks of two parties
std::vector<uint64_t> rK1_global(toeplitzWords), rK2_global(toeplitzWords), rK_global(toeplitzWords);
PackedZ2<N_COLS> rx1_global, rx2_global, rx_global; //mask for input
PackedZ2<N_COLS> sw1_global, sw2_global, sw_global; //sw = rK * rx + rw
PackedZ2<N_COLS> rw1_global, rw2_global, rw_global; //random

//declare a variable in Z3 called r0z and r1z
PackedZ3<N_SIZE>r0z_global, r1z_global;
PackedZ3<N_SIZE> r0z1_global, r0z2_global, r1z1_global, r1z2_global;


/*
 * Preprocessing: Generates rx(and its shares rx1, rx2), rK(its shares rK1, rK2),
 * sw( and its shares sw1, sw2)
 */
void preProc_mod2_dm2020(unsigned int nTimes)
{
#ifdef DEBUG
    std::cout<<"newprotocol.cpp/preProc_mod2_dm_2020: Preprocessing begins"<<std::endl;
#endif

    //1.generate mod2 random values for rK, rx and sw
    for(unsigned int i=0; i<nTimes; i++)
    {
        //2.generate random rx1, rx2 and rx = rx1 ^ rx2
        rx1_global.randomize(); // random rx[i]'s
        rx2_global.randomize();
        rx_global = rx1_global; //rx = rx1
        rx_global ^= rx2_global;
        //rx_global.add(rx2_global); //rx = rx1 ^ rx2

        //3.generate random rw1, rw2 and rw = rw1 ^ rw2
        rw1_global.randomize(); // random sw1[i]
        rw2_global.randomize(); // random sw2[i]
        rw_global = rw1_global; //rw = rw1
        rw_global ^= rw2_global;
        //rw_global.add(rw2_global); //rw += rw2

        //4.generate rK1, rK2, rK = rK1 ^ rK2
        for (auto& w : rK1_global) w = randomWord(); //creating 8 random vector for rk1
        rK1_global[rK1_global.size() - 1] &= topelitzMask;

        for (auto& w : rK2_global) w = randomWord(); //creating 8 random vector for rk2
        rK2_global[rK2_global.size() - 1] &= topelitzMask;

        for (int i = 0; i < rK_global.size(); i++)
        {
            rK_global[i] = rK1_global[i] ^ rK2_global[i];   //rk = rk1 ^ rk2
        }

        //5.Calculate sw = rk_global * rx_global ^ rw_global
        sw1_global.toeplitzByVec(rK1_global,rx1_global);
        sw1_global ^= rw1_global;
        sw2_global.toeplitzByVec(rK2_global,rx2_global);
        sw2_global ^= rw2_global;
        sw_global = sw1_global;
        sw_global ^= sw2_global;
        //sw_global.add(sw2_global);

    }

#ifdef DEBUG
    std::cout<<"newprotocol.cpp/preProc_mod2_dm_2020: Preprocessing ends"<<std::endl;
#endif
}

void preProc_mod3_dm2020(unsigned int nTimes)
{
    //Cast rw and 1-rw  to mod3
    int sw_val,not_sw_val;
    for(int z3_count = 0; z3_count < N_SIZE; z3_count++)
    {
        sw_val = (unsigned int)sw_global.at(z3_count);
        not_sw_val = 1 - sw_val;
        r0z_global.first.set(z3_count,0);   //r0z_msb = 0
        r0z_global.second.set(z3_count, sw_val); //r0z_lsb = rw
        r1z_global.first.set(z3_count, 0);  //r1z_msb = 0
        r1z_global.second.set(z3_count, not_sw_val);    //r1z_lsb = rw
    }

    //generate random values for r0z1 and r1z1; share of r0z and r1z for party 1
    r0z1_global.randomize();
    r1z1_global.randomize();

    //perform xor operation and calculate r0z2 and r1z2; share of r0z and r1z for party 2
    r0z2_global = r0z1_global;    //r0z2 = r0z ^ r0z1
    r0z2_global ^= r0z_global;

    r1z2_global = r1z1_global;    //r1z2 = r1z ^ r1z1
    r1z2_global ^= r1z_global;
}


void fetchPreproc_party1(PackedZ2<N_COLS>& rx1, PackedZ2<N_COLS>& rw1, PackedZ2<N_COLS>& sw1,
                         std::vector<uint64_t> rK1, PackedZ3<N_SIZE> r0z1, PackedZ3<N_SIZE> r1z1)
{
    rx1 = rx1_global;
    rK1 = rK1_global;
    rw1 = rw1_global;
    sw1 = sw1_global;
    r0z1 = r0z1_global;
    r1z1 = r1z1_global;
}

void fetchPreproc_party2(PackedZ2<N_COLS>& rx2, PackedZ2<N_COLS>& rw2, PackedZ2<N_COLS>& sw2,
                         std::vector<uint64_t> rK2, PackedZ3<N_SIZE> r0z2, PackedZ3<N_SIZE> r1z2)
{
    rx2 = rx2_global;
    rK2 = rK2_global;
    rw2 = rw2_global;
    sw2 = sw2_global;
    r0z2 = r0z2_global;
    r1z2 = r1z2_global;
}

void party1_round_1(PackedZ2<N_COLS> x1_mask, std::vector<uint64_t> K1_mask,
                    PackedZ2<N_COLS>& x1, PackedZ2<N_COLS>& rx1, std::vector<uint64_t> K1, std::vector<uint64_t> rK1)
{
    x1_mask = x1;
    x1_mask ^= rx1_global;
    for(int word_count=0; word_count < K1.size(); word_count++) {
        K1_mask[word_count] = K1[word_count] ^ rK1[word_count];
    }
}

void party2_round_1(PackedZ2<N_COLS> x2_mask, std::vector<uint64_t> K2_mask,
                    PackedZ2<N_COLS>& x2, PackedZ2<N_COLS>& rx2, std::vector<uint64_t> K2, std::vector<uint64_t> rK2)
{
    x2_mask = x2;
    x2_mask ^= rx1_global;
    for(int word_count=0; word_count < K2.size(); word_count++) {
        K2_mask[word_count] = K2[word_count] ^ rK2[word_count];
    }
}

void compute_input_mask(PackedZ2<N_COLS> x_mask, std::vector<uint64_t> K_mask,PackedZ2<N_COLS> x1_mask,
                  PackedZ2<N_COLS> x2_mask,std::vector<uint64_t> K1_mask,std::vector<uint64_t> K2_mask)
{
    x_mask = x1_mask;
    x_mask ^= x2_mask;

    for(int word_count=0; word_count < K_mask.size(); word_count++)
    {
        K_mask[word_count] = K1_mask[word_count] ^ K2_mask[word_count];
    }
}

void party1_round2(PackedZ2<N_COLS> w1_mask, std::vector<uint64_t> K_mask, PackedZ2<N_COLS> x_mask, PackedZ2<N_COLS> rx1,
        std::vector<uint64_t> rK1, PackedZ2<N_COLS> sw1)
{
    PackedZ2<N_COLS> x_rx1 = x_mask;    //x_rx1 = x_mask - rx1
    x_rx1 ^= rx1_global;

    PackedZ2<N_COLS> Kx1;   //kx1 = K_mask * x_rx1
    Kx1.toeplitzByVec(K_mask,x_rx1);

    PackedZ2<N_COLS> x_rK1;//x_rk1 = rK1 * x_mask
    x_rK1.toeplitzByVec(rK1_global,x_mask);

    w1_mask = Kx1;//w1 = kx1 - rk_x1
    w1_mask ^= x_rK1;
    w1_mask ^= sw_global;
}
void party2_round2(PackedZ2<N_COLS> w2_mask, std::vector<uint64_t> K_mask,PackedZ2<N_COLS> x_mask, PackedZ2<N_COLS> rx2,
                   std::vector<uint64_t> rK2, PackedZ2<N_COLS> sw2)
{
    PackedZ2<N_COLS> x_rx2 = x_mask;    //x_rx2 = x_mask - rx2
    x_rx2 ^= rx2_global;

    PackedZ2<N_COLS> Kx2;   //kx2 = K_mask * x_rx2
    Kx2.toeplitzByVec(K_mask,x_rx2);

    PackedZ2<N_COLS> x_rK2; //x_rk2 = rK2 * x_mask
    x_rK2.toeplitzByVec(rK2_global,x_mask);

    w2_mask = Kx2; //w2 = kx2 - rk_x2
    w2_mask ^= x_rK2;
    w2_mask ^= sw_global;
}

void compute_wmask(PackedZ2<N_COLS> w_mask, PackedZ2<N_COLS> w1_mask, PackedZ2<N_COLS> w2_mask)
{
    w_mask = w1_mask;
    w_mask ^= w2_mask;
}

void party1_round3(PackedZ3<81> y1_z3,PackedZ3<N_SIZE> r0z1,
                   PackedZ3<N_SIZE> r1z1, std::vector<PackedZ3<81> > Rmat,PackedZ2<N_COLS> w_mask)
{
    //PARTY 1: calculates the value of mux(w', r0z1, r1z1)
    PackedZ3<N_SIZE> res1;
    res1 = r0z1; //copy the contents of r0z to res1.

    //perform the mux functionality, pass the Packedz3 and converted vector of w_mask
    res1.mux(r1z1, w_mask.bits);

    //party 1 computes y1 =  M * res1
    y1_z3.matByVec(Rmat, res1);
}

void party2_round3(PackedZ3<81> y2_z3,PackedZ3<N_SIZE> r0z2,
                   PackedZ3<N_SIZE> r1z2, std::vector<PackedZ3<81> > Rmat,PackedZ2<N_COLS> w_mask)
{
    //party 2 calculates the value of mux(w', r0z, r1z)
    PackedZ3<N_SIZE> res2;
    res2 = r0z2;

    //perform the mux functionality, pass the Packedz3 and converted vector of w_mask
    res2.mux(r1z2, w_mask.bits);

    //party computes y2 = M* res2
    y2_z3.matByVec(Rmat, res2);
}
void compute_y_out(PackedZ3<81>  y_out_z3, PackedZ3<81> y1_z3, PackedZ3<81> y2_z3)
{
    //calculating y = y1 + y2
    y_out_z3 = y1_z3;
    y_out_z3 ^= y2_z3;
}

/*
 * This function generates the variables and perform computation to simulate centralized
 * version of the new protocol
 */
void PRF_new_protocol_central()
{
    //local declarations=======9/15
    std::vector<uint64_t> rK1(toeplitzWords), rK2(toeplitzWords), rK(toeplitzWords);
    PackedZ2<N_COLS> rx1, rx2, rx; //mask for input
    PackedZ2<N_COLS> sw1, sw2, sw; //sw = rK * rx + rw
    PackedZ2<N_COLS> rw1, rw2, rw;

    //declaring the inputs
    std::vector<uint64_t> K1(toeplitzWords), K2(toeplitzWords);
    PackedZ2<N_COLS> x1, x2;

    //declare a variable in Z3 called r0z and r1z
    PackedZ3<N_SIZE>r0z, r1z;
    PackedZ3<N_SIZE> r0z1, r0z2, r1z1, r1z2;

    //============================

    int nTimes = 1;

    //1. Perform preprocessing
    preProc_mod2_dm2020(nTimes);//it was nRuns * 2 in previous protocol, needs to be changed later
    preProc_mod3_dm2020(nTimes);

    fetchPreproc_party1(rx1,rw1,sw1,rK1,r0z1,r1z1);
    fetchPreproc_party2(rx2,rw2,sw2,rK2, r0z2,r1z2);


    //2. generate input X and key K
    randomWord(1); // use seed=1
    for (auto &w : K1) w = randomWord();
    K1[K1.size() - 1] &= topelitzMask; // turn off extra bits at the end
    for (auto &w : K2) w = randomWord();
    K2[K2.size() - 1] &= topelitzMask; // turn off extra bits at the end
    x1.randomize();
    x2.randomize();



    //3. Parties locally compute [x'] = [x] + [rx] and [K'] = [K] + [rk]
    PackedZ2<N_COLS> x1_mask,x_mask, x2_mask;
    std::vector<uint64_t> K1_mask(toeplitzWords), K2_mask(toeplitzWords), K_mask(toeplitzWords);

    party1_round_1(x1_mask,K1_mask,x1,rx1,K1,rK1);
    party2_round_1(x2_mask,K2_mask,x2,rx2,K2,rK2);

    //both the parties are supposed to exchange their mask values
    compute_input_mask(x_mask, K_mask, x1_mask, x2_mask, K1_mask, K2_mask);


#ifdef DEBUG
    std::cout<<"newprotocol.cpp/PRF_new_protocol_central(): Round 1 ends"<<std::endl;
#endif



#ifdef DEBUG
    std::cout<<"newprotocol.cpp/PRF_new_protocol_central(): Round 2 begins"<<std::endl;
#endif

    //Round 2: Both parties compute w' = [k'x'] - k'[rx] - rk.x' + [rk * rk + rw]

    PackedZ2<N_COLS> w1_mask, w2_mask, w_mask; //w' = K'(x' - rx) - rK'*x' + sw
    party1_round2(w1_mask, K_mask,x_mask, rx1, rK1, sw1);
    party2_round2(w2_mask, K_mask,x_mask, rx1, rK1, sw1);

    compute_wmask(w_mask, w1_mask, w2_mask);

#ifdef DEBUG
    std::cout<<"newprotocol.cpp/PRF_new_protocol_central(): Round 2 ends"<<std::endl;
#endif


#ifdef DEBUG
    std::cout<<"newprotocol.cpp/PRF_new_protocol_central(): Round 3 begins"<<std::endl;
#endif

    //Round 3: Both parties compute w' = [k'x'] - k'[rx] - rk.x' + [rk * rk + rw]

    PackedZ3<N_SIZE> z1, z2;
    PackedZ3<N_SIZE> res1, res2, res; //stores the mux result of both the parties
    PackedZ3<81> y1_z3, y2_z3, y_out_z3;

    //generate a 81 X 256 randomization matrix in Z3.
    std::vector<PackedZ3<81> > Rmat(256);
    for (auto &col : Rmat) // iterate over the columns
        col.randomize();

    party1_round3(y1_z3,r0z1,r1z1,Rmat,w_mask);
    party2_round3(y2_z3,r0z2,r1z2,Rmat,w_mask);

    compute_y_out(y_out_z3,y1_z3,y2_z3);

    //compute res =  res1 + res2; res is equivalent to z
    res = res1;
    res ^= res2;

#ifdef DEBUG
    std::cout<<"newprotocol.cpp/PRF_new_protocol_central(): Round 3 ends"<<std::endl;
#endif
#ifdef DEBUG
    std::cout<<"newprotocol.cpp/PRF_new_protocol_central(): Calling PRF unit test function"<<std::endl;
#endif
    PackedZ3<81> test_out1_z3, test_out2_z3;
    //PRF_packed_test(K1,x1,K2,x2,Rmat,test_out1_z3,test_out2_z3,1);
    //PRF_packed_centralized_test(K1,x1,K2,x2,Rmat,test_out1_z3,test_out2_z3,1);
    PRF_packed_centralized_test(K1,x1,K2,x2,Rmat,test_out1_z3,test_out2_z3,1);

    PackedZ3<81>test_out_z3 = test_out1_z3;
    test_out_z3.add(test_out2_z3);//out = out1 + out2

    //comparing test_out_z3 with y_out_z3
    bool test_flag = 0;//no problem
    for(int test_cnt = 0; test_cnt < 81; test_cnt++)
    {
        if(test_out_z3.at(test_cnt) != y_out_z3.at(test_cnt))
        {
            std::cout<<"The problem is at index "<<test_cnt<<std::endl;
            test_flag = 1;//some problem
            break;
        }
    }
    if(test_flag == 0)
        std::cout<<std::endl<<"New protocol test: Test passed"<<std::endl;
    else
        std::cout<<std::endl<<"New protocol test: Test fails"<<std::endl;

#ifdef NP_TEST
    std::cout<<"newprotocol.cpp/PRF_new_protocol_central(): NP_TEST is enabled; calling the test function";
#endif
}

