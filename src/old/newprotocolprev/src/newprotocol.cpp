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

// in Toeplitz-by-x.hpp
// #define N_ROWS 256
// #define N_COLS 256

//using namespace std;


// Declaring the Keys and inputs of two parties (GLOBAL)
std::vector<uint64_t> K1(toeplitzWords), K2(toeplitzWords);
PackedZ2<N_COLS> x1, x2;

//Declaring shares of key and input masks of two parties
static std::vector< std::vector<uint64_t> > rK1_global, rK2_global, rK_global;
static std::vector< PackedZ2<N_COLS> > rx1_global, rx2_global, rx_global;
static std::vector< PackedZ2<N_COLS> >  sw1_global, sw2_global, sw_global, rw_global;

//declare a variable in Z3 called r0z and r1z
static std::vector< PackedPairZ2<N_SIZE> > r0z, r1z;

static std::vector<PackedZ2<N_COLS>> x1_mask, x2_mask;
static std::vector< std::vector<uint64_t> > K1_mask, K2_mask;



/*
 * Preprocessing: Generates rx(and its shares rx1, rx2), rK(its shares rK1, rK2),
 * sw( and its shares sw1, sw2)
 */
void preProc_mod2_dm2020(unsigned int nTimes)
{
    std::cout<<"newprotocol.cpp/preProc_mod2_dm_2020:Preprocessing centralized"<<std::endl;
    rK_global.resize(nTimes, std::vector<uint64_t>(toeplitzWords));//mask for key
    rx_global.resize(nTimes);//mask for input
    rw_global.resize(nTimes);
    sw_global.resize(nTimes);

    rx1_global.resize(nTimes);
    rx2_global.resize(nTimes);
    rK1_global.resize(nTimes);
    rK2_global.resize(nTimes);
    sw1_global.resize(nTimes);
    sw2_global.resize(nTimes);
    r0z.resize(nTimes);
    r1z.resize(nTimes);

    //1.generate mod2 random values for rK, rx and sw
    for(unsigned int i=0; i<nTimes; i++)
    {
        //2.generate random rx1, rx2 and rx = rx1 ^ rx2
        rx1_global[i].randomize(); // random rx[i]'s
        rx2_global[i].randomize();
        rx_global[i].add(rx1_global[i]);
        rx_global[i].add(rx2_global[i]);

        //3.generate random sw1, sw2 and sw = sw1 ^ sw2
        sw1_global[i].randomize(); // random sw1[i]
        sw2_global[i].randomize(); // random sw2[i]
        sw_global[i].add(sw1_global[i]);
        sw_global[i].add(sw2_global[i]);

        #ifdef DEBUG
        std::cout<<"size of rk"<<rK_global[i].size()<<std::endl;
        #endif

        //4.generate rK1, rK2, rK = rK1 ^ rK2
        for (auto& w : rK1_global[i]) w = randomWord(); //creating 8 random vector for rk1
        for (auto& w : rK2_global[i]) w = randomWord(); //creating 8 random vector for rk2
        std::cout<<rK_global[i].size()<<std::endl;
        for(int j = 0; j <  rK_global[i].size(); j++)
        {
            rK_global[i][j] = rK1_global[i][j] ^ rK2_global[i][j];  //rk = rk1 ^ rk2
        }
        //std::cout<<"The size of rx is "<<rx_global.size()<<std::endl;

        //5.Calculate rw = rk_global * rx_global ^ sw_global
        rw_global[i].toeplitzByVec(rK_global[i],rx_global[i]);
        rw_global[i] ^= sw_global[i];

        //print rw and 1-rw and cast both to mod3
        #ifdef DEBUG
        std::cout<<rw_global[i]<<std::endl;
        #endif
        for(int z3_count = 0; z3_count < r0z.size(); z3_count++)
        {
            int rw_val = (unsigned int)rw_global[i].at(z3_count);
            int not_rw_val = 1 - rw_val;
            r0z[i].first.set(z3_count,0);   //r0z_msb = 0
            r0z[i].second.set(z3_count, rw_val); //r0z_lsb = rw
            r1z[i].first.set(z3_count, 0);  //r1z_msb = 0
            r1z[i].second.set(z3_count, not_rw_val);    //r1z_lsb = rw
        }

    }


}

/*
 * This function generates the variables and perform computation to simulate centralized
 * version of the new protocol
 */
void PRF_new_protocol_central()
{

    std::cout<<"newprotocol.cpp/PRF_new_protocol_central() start"<<std::endl<<std::flush;
    //1. Perform preprocessing
    int nTimes = 1;
    preProc_mod2_dm2020(nTimes);//it was nRuns * 2 in previous protocol, needs to be changed later

    std::cout<<"newprotocol.cpp/Finished preProc"<<std::endl;
    //2. generate input X and key K
    randomWord(1); // use seed=1
    for (auto &w : K1) w = randomWord();
    K1[K1.size() - 1] &= topelitzMask; // turn off extra bits at the end

    for (auto &w : K2) w = randomWord();
    K2[K2.size() - 1] &= topelitzMask; // turn off extra bits at the end

    x1.randomize();
    x2.randomize();

    std::cout.flush();
    std::cout<<"newprotocol.cpp: Preprocessing complete"<<std::endl;

    //3. Parties locally compute [x'] = [x] + [rx] and [K'] = [K] + [rk]
    for(int n_count =0; n_count < nTimes; n_count++)
    {
        //Party 1: masking input (x' = x ^ rx)
        x1_mask.resize(nTimes);//x1_mask is x1'
        x1_mask[n_count].add(x1);
        x1_mask[n_count].add(rx1_global[n_count]);

        //Party 1 & 2: masking key K' = K ^ rK
        /*for(int word_count=0; word_count < rK_global.size(); word_count++)
        {
            K1_mask[n_count][word_count] = K1[n_count][word_count] ^ rK1_global[n_count][word_count];
            K2_mask[n_count][word_count] = K2[n_count][word_count] ^ rK2_global[n_count][word_count];
        }*/

        //Party 2: masking input
        x2_mask.resize(nTimes);//x2_mask is x2'
        x2_mask[n_count].add(x2);
        x2_mask[n_count].add(rx2_global[n_count]);
        
    }

    //(rx1_global);


    //Both parties compute w' = [k'x'] - k'[rx] - rk.x' + [rk * rk + rw]
}