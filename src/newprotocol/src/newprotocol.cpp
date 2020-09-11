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
std::vector<uint64_t> rK1_global(toeplitzWords), rK2_global(toeplitzWords), rK_global(toeplitzWords);
PackedZ2<N_COLS> rx1_global, rx2_global, rx_global;
PackedZ2<N_COLS> sw1_global, sw2_global, sw_global;
PackedZ2<N_COLS> rw1_global, rw2_global, rw_global;

//declare a variable in Z3 called r0z and r1z
PackedPairZ2<N_SIZE>r0z, r1z;

PackedZ2<N_COLS> x1_mask, x2_mask,x_mask;
std::vector<uint64_t> K1_mask(toeplitzWords), K2_mask(toeplitzWords), K_mask(toeplitzWords);



/*
 * Preprocessing: Generates rx(and its shares rx1, rx2), rK(its shares rK1, rK2),
 * sw( and its shares sw1, sw2)
 */
void preProc_mod2_dm2020(unsigned int nTimes)
{
    std::cout<<"newprotocol.cpp/preProc_mod2_dm_2020:Preprocessing centralized"<<std::endl;
    //rK_global.resize(nTimes);//mask for key
    //rx_global.resize(nTimes);//mask for input

    //resizing the shares
    //rK1_global.resize(nTimes);
    //rK2_global.resize(nTimes);


    //1.generate mod2 random values for rK, rx and sw
    for(unsigned int i=0; i<nTimes; i++)
    {
        //2.generate random rx1, rx2 and rx = rx1 ^ rx2
        rx1_global.randomize(); // random rx[i]'s
        rx2_global.randomize();
        rx_global = rx1_global; //rx = rx1
        //rx_global[i].add(rx1_global[i]);
        rx_global.add(rx2_global); //rx = rx1 ^ rx2

        //3.generate random rw1, rw2 and rw = rw1 ^ rw2
        rw1_global.randomize(); // random sw1[i]
        rw2_global.randomize(); // random sw2[i]
        rw_global = rw1_global; //rw = rw1
        //rw_global[i].add(rw1_global[i]); //rw += rw1
        rw_global.add(rw2_global); //rw += rw2

        #ifdef DEBUG
        std::cout<<"size of rk "<<rK_global.size()<<std::endl;
        #endif

        //4.generate rK1, rK2, rK = rK1 ^ rK2
        for (auto& w : rK1_global) w = randomWord(); //creating 8 random vector for rk1
        rK1_global[rK1_global.size() - 1] &= topelitzMask;

        for (auto& w : rK2_global) w = randomWord(); //creating 8 random vector for rk2
        rK2_global[rK2_global.size() - 1] &= topelitzMask;

        for (int i = 0; i < rK_global.size(); i++)
        {
            rK_global[i] = rK1_global[i] ^ rK2_global[i];   //rk = rk1 ^ rk2
        }


        //std::cout<<"The size of rx is "<<rx_global.size()<<std::endl;

        //5.Calculate sw = rk_global * rx_global ^ rw_global
        sw1_global.toeplitzByVec(rK1_global,rx1_global);
        sw1_global ^= rw1_global;
        sw2_global.toeplitzByVec(rK2_global,rx2_global);
        sw2_global ^= rw2_global;
        sw_global = sw1_global;
        sw_global.add(sw2_global);

        //print rw and 1-rw and cast both to mod3
        #ifdef DEBUG
        std::cout<<rw_global<<std::endl;
        #endif

        int rw_val,not_rw_val;
        for(int z3_count = 0; z3_count < N_SIZE; z3_count++)
        {
            rw_val = (unsigned int)rw_global.at(z3_count);
            not_rw_val = 1 - rw_val;
            r0z.first.set(z3_count,0);   //r0z_msb = 0
            r0z.second.set(z3_count, rw_val); //r0z_lsb = rw
            r1z.first.set(z3_count, 0);  //r1z_msb = 0
            r1z.second.set(z3_count, not_rw_val);    //r1z_lsb = rw
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

    std::cout<<"newprotocol.cpp: Preprocessing complete"<<std::endl;

    //3. Parties locally compute [x'] = [x] + [rx] and [K'] = [K] + [rk]
    //Party 1: masking input (x' = x ^ rx)
    x1_mask = x1;
    x1_mask.add(rx1_global);

    //Party 1 & 2: masking key K' = K ^ rK
    for(int word_count=0; word_count < K1.size(); word_count++)
    {
        K1_mask[word_count] = K1[word_count] ^ rK1_global[word_count];
        K2_mask[word_count] = K2[word_count] ^ rK2_global[word_count];
    }

    //Party 2: masking input
    x2_mask.add(x2);
    x2_mask.add(rx2_global);
    #ifdef DEBUG
        std::cout<<"x1_mask: "<<x1_mask<<std::endl;
    #endif

    //Both parties compute w' = [k'x'] - k'[rx] - rk.x' + [rk * rk + rw]
}

