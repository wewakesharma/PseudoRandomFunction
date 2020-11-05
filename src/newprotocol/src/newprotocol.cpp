//
// Created by Vivek Sharma on 9/2/20.
//

#include <iostream>
#include "newprotocol.hpp"
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
#include "newprotocol_test.hpp"
#include "PRF.hpp"
#include "lookup_functions.h"

long timer_round1_p1 = 0;
long timer_round1_p2 = 0;
long timer_round1_mask = 0;

long timer_round2_p1 = 0;
long timer_round2_p2 = 0;
long timer_round2_mask = 0;

long timer_round3_p1 = 0;
long timer_round3_p2 = 0;

long timer_round1 = 0;
long timer_round2 = 0;
long timer_round3 = 0;
long timer_round3_mux = 0;
long timer_round3_lookup = 0;

long timer_np_prf = 0;

// Declaring the global variables for lookup implementation
std::vector<std::vector<PackedZ3<81> > > Rmat16(16); //Rmat16 vector of size 16 X 16 X 81 (GLOBAL)
std::vector<std::vector<PackedZ3<81> > > lookup_table(16); //lookup table

//Declaring shares of key and input masks of two parties
std::vector<uint64_t> rK1_global(toeplitzWords), rK2_global(toeplitzWords), rK_global(toeplitzWords);
PackedZ2<N_COLS> rx1_global, rx2_global, rx_global; //mask for input
PackedZ2<N_COLS> sw1_global, sw2_global, sw_global; //sw = rK * rx + rw
PackedZ2<N_COLS> rw1_global, rw2_global, rw_global; //random

//declare a variable in Z3 called r0z and r1z
PackedZ3<N_SIZE>r0z_global, r1z_global;
PackedZ3<N_SIZE> r0z1_global, r0z2_global, r1z1_global, r1z2_global;

PackedZ3<N_SIZE> res1_global, res2_global;

/*
 * Preprocessing: Generates rx(and its shares rx1, rx2), rK(its shares rK1, rK2),
 * sw( and its shares sw1, sw2)
 */

PackedZ2<N_COLS> fetch_rw_global()
{
    return rw_global;
}

void preProc_mod2_dm2020(unsigned int nRuns)
{
#ifdef PRINT_VAL//preprocessing begins
    std::cout<<"newprotocol.cpp/preProc_mod2_dm_2020: Preprocessing begins \n"<<std::endl;
#endif

    //1.generate mod2 random values for rK, rx and sw
    for(unsigned int i=0; i<nRuns; i++)
    {
        //2.generate random rx1, rx2 and rx = rx1 ^ rx2
        rx1_global.randomize(); // random rx[i]'s
        rx2_global.randomize();

        //4.generate rK1, rK2, rK = rK1 ^ rK2
        for (auto& w : rK1_global) w = randomWord(); //creating 8 random vector for rk1
        rK1_global[rK1_global.size() - 1] &= topelitzMask;

        for (auto& w : rK2_global) w = randomWord(); //creating 8 random vector for rk2
        rK2_global[rK2_global.size() - 1] &= topelitzMask;

        sw1_global.randomize(); // random sw1[i]
        sw2_global.randomize(); // random sw2[i]

#ifdef DEBUG //sets rk1, rK2, rx1, rx2, sw1, sw2
        rK1_global= {0,0,0,0,0,0,0,0};
        rK2_global = {0,0,0,0,0,0,0,0};

        rx1_global.reset();
        rx2_global.reset();
     //   rx1_global.set(0,1);
     //   rx2_global.set(1,1);
      //  rx2_global.set(2,1);
      //  rx2_global.set(33,1);

        sw1_global.reset();
        sw2_global.reset();
   //     sw1_global.set(0,1);
    //    sw2_global.set(1,1);

#endif
        rx_global = rx1_global; //rx = rx1
        rx_global ^= rx2_global;

        //3.generate random sw1,sw2 and sw = sw1 ^ sw2
        sw_global = sw1_global; //sw = sw1
        sw_global ^= sw2_global;

        for (int i = 0; i < rK_global.size(); i++)
        {
            rK_global[i] = rK1_global[i] ^ rK2_global[i];   //rk = rk1 ^ rk2
        }

        //5.Calculate rw = rk_global * rx_global ^ sw_global
        rw_global.toeplitzByVec(rK_global,rx_global);
        rw_global ^= sw_global;
    }
#ifdef PRINT_VAL//prints the value of rw = rk*rx + sw
        std::cout<<"rw_global "<<rw_global<<std::endl;
        std::cout<<"rw1_global "<<rw1_global<<std::endl;
        std::cout<<"rw2_global "<<rw2_global<<std::endl;
        std::cout<<"newprotocol.cpp/preProc_mod2_dm_2020: Preprocessing ends \n"<<std::endl;
#endif
}

void preProc_mod3_dm2020(unsigned int Runs)//generate r0z and r1z and share them between two parties
{
    //Cast rw and 1-rw  to mod3; single line assignment of r0z_global and r1z_global
    std::vector<unsigned int> rw_int; //array to act as a bridge between packedZ2 to packedZ3 conversion
    rw_global.toArray(rw_int); //convert packedZ2 to an array
    r0z_global.fromArray(rw_int); //convert an array to packedZ3

    std::vector<unsigned int> not_rw_int;//to store ~sw_val
    PackedZ2<N_COLS> not_rw_global;//to store ~sw_val
    not_rw_global = rw_global; //copy the value of sw_global
    not_rw_global.negate(); //negate the values
    not_rw_global.toArray(not_rw_int); //convert the values in an array
    r1z_global.fromArray(not_rw_int);   //convert array into packedZ3 for further processing

    //generate random values for r0z1 and r1z1; share of r0z and r1z for party 1
    r0z1_global.randomize();
    r1z1_global.randomize();

    #ifdef DEBUG //sets r0z1, r0z2, r1z1, r1z2
        r0z1_global.reset();
        r0z1_global.set(0,2);
        r0z1_global.set(2,2);
        r1z1_global.reset();
        r1z1_global.set(1,2);
    #endif
    //perform xor operation and calculate r0z2 and r1z2; share of r0z and r1z for party 2
    r0z2_global = r0z_global;    //r0z2 = r0z ^ r0z1
    r0z2_global.subtract(r0z1_global);

    r1z2_global = r1z_global;    //r1z2 = r1z ^ r1z1
    r1z2_global.subtract(r1z1_global);


#ifdef PRINT_VAL//print the values of r0z, r0z1  and r0z2
    std::cout<<"newprotocol.cpp/preProc_mod3_dm2020():"<<std::endl;
    std::cout<<"sw_global "<<rw_global<<std::endl;
    std::cout<<"r0z "<<r0z_global<<std::endl;
    std::cout<<"r0z1 "<<r0z1_global<<std::endl;
    std::cout<<"r0z2 "<<r0z2_global<<std::endl;
    std::cout<<"not_sw_global "<<not_rw_global<<std::endl;
    std::cout<<"r1z "<<r1z_global<<std::endl;
    std::cout<<"r1z1 "<<r1z1_global<<std::endl;
    std::cout<<"r1z2 "<<r1z2_global<<"\n"<<std::endl;

#endif

}

void fetchPreproc_party1(PackedZ2<N_COLS>& rx1, PackedZ2<N_COLS>& rw, PackedZ2<N_COLS>& sw1,
                         std::vector<uint64_t>& rK1, PackedZ3<N_SIZE>& r0z1, PackedZ3<N_SIZE>& r1z1)
{
    rx1 = rx1_global;
    rK1 = rK1_global;
    rw = rw_global;
    sw1 = sw1_global;
    r0z1 = r0z1_global;
    r1z1 = r1z1_global;
}

void fetchPreproc_party2(PackedZ2<N_COLS>& rx2, PackedZ2<N_COLS>& rw, PackedZ2<N_COLS>& sw2,
                         std::vector<uint64_t>& rK2, PackedZ3<N_SIZE>& r0z2, PackedZ3<N_SIZE>& r1z2)
{
    rx2 = rx2_global;
    rK2 = rK2_global;
    rw = rw_global;
    sw2 = sw2_global;
    r0z2 = r0z2_global;
    r1z2 = r1z2_global;
}

void party1_round_1(PackedZ2<N_COLS>& x1_mask, std::vector<uint64_t>& K1_mask,
                    PackedZ2<N_COLS>& x1, PackedZ2<N_COLS>& rx1, std::vector<uint64_t>& K1, std::vector<uint64_t>& rK1)
{
    x1_mask = x1;
    x1_mask.add(rx1);
    for(int word_count=0; word_count < K1.size(); word_count++) {
        K1_mask[word_count] = K1[word_count] ^ rK1[word_count];
    }

    #ifdef DEBUG
        std::cout<<"newprotocol.cpp/party1_round_1():"<<std::endl;
        std::cout<<"x1 "<<x1<<std::endl;
        std::cout<<"K1 "<<K1<<std::endl;
        std::cout<<"rK1 "<<rK1<<std::endl;
        std::cout<<"x1_mask "<<x1_mask<<std::endl;
        std::cout<<"K1_mask "<<K1_mask<<std::endl;
    #endif
}

void party2_round_1(PackedZ2<N_COLS>& x2_mask, std::vector<uint64_t>& K2_mask,
                    PackedZ2<N_COLS>& x2, PackedZ2<N_COLS>& rx2, std::vector<uint64_t>& K2, std::vector<uint64_t>& rK2)
{
    x2_mask = x2;
    x2_mask ^= rx2;

    for(int word_count=0; word_count < K2.size(); word_count++) {
        K2_mask[word_count] = K2[word_count] ^ rK2[word_count];
    }

#ifdef PRINT_VAL
    std::cout<<"newprotocol.cpp/party2_round_1():"<<std::endl;
    std::cout<<"x2 "<<x2<<std::endl;
    std::cout<<"K2 "<<K2<<std::endl;
    std::cout<<"rK2 "<<rK2<<std::endl;
    std::cout<<"x2_mask "<<x2_mask<<std::endl;
    std::cout<<"K2_mask "<<K2_mask<<"\n"<<std::endl;
#endif
}

//compute the XOR of input mask of both the parties: x_mask = x1_mask ^ x2_mask; K_mask = K1_mask ^ K2_mask
void compute_input_mask(PackedZ2<N_COLS>& x_mask, std::vector<uint64_t>& K_mask,PackedZ2<N_COLS>& x1_mask,
                  PackedZ2<N_COLS>& x2_mask,std::vector<uint64_t>& K1_mask,std::vector<uint64_t>& K2_mask)
{
    x_mask = x1_mask; //x=x1 ^ x2
    x_mask ^= x2_mask;

    for(int word_count=0; word_count < K_mask.size(); word_count++)//K = K1 ^ K2
    {
        K_mask[word_count] = K1_mask[word_count] ^ K2_mask[word_count];
    }
#ifdef PRINT_VAL
    std::cout<<"newprotocol.cpp/compute_input_mask(): "<<std::endl;
    std::cout<<"x_mask "<<x_mask<<std::endl;
    std::cout<<"K_mask "<<K_mask<<"\n"<<std::endl;
#endif
}

void party1_round2(PackedZ2<N_COLS>& w1_mask, std::vector<uint64_t>& K_mask, PackedZ2<N_COLS>& x_mask, PackedZ2<N_COLS>& rx1,
        std::vector<uint64_t>& rK1, PackedZ2<N_COLS>& x1, PackedZ2<N_COLS>& sw1)
{
    //new code to compute w1_mask
    PackedZ2<N_COLS> K_mask_x1;
    K_mask_x1.toeplitzByVec(K_mask,x1);

    PackedZ2<N_COLS> rK1_x_mask;
    rK1_x_mask.toeplitzByVec(rK1,x_mask);

    w1_mask = K_mask_x1;
    w1_mask ^= rK1_x_mask;
    w1_mask ^= sw1;

    //previous buggy code to compute w1_mask
    /*PackedZ2<N_COLS> x_rx1 = x_mask;    //x_rx1 = x_mask - rx1
    x_rx1 ^= rx1;

    PackedZ2<N_COLS> Kx1;   //kx1 = K_mask * x_rx1
    Kx1.toeplitzByVec(K_mask,x_rx1);

    PackedZ2<N_COLS> x_rK1;//x_rk1 = rK1 * x_mask
    x_rK1.toeplitzByVec(rK1,x_mask);

    w1_mask = Kx1;//w1 = kx1 - rk_x1
    w1_mask ^= x_rK1;
    w1_mask ^= sw1;*/
#ifdef PRINT_VAL
    std::cout<<"newprotocol.cpp/party1_round2(): "<<std::endl;
    std::cout<<"K_mask * x1"<<K_mask_x1<<std::endl;
    std::cout<<"rK1 * x_mask"<<rK1_x_mask<<std::endl;
    std::cout<<"w1_mask "<<w1_mask<<"\n"<<std::endl;
#endif
}
void party2_round2(PackedZ2<N_COLS>& w2_mask, std::vector<uint64_t>& K_mask,PackedZ2<N_COLS>& x_mask, PackedZ2<N_COLS>& rx2,
                   std::vector<uint64_t>& rK2, PackedZ2<N_COLS>& x2, PackedZ2<N_COLS>& sw2)
{
    //new code to compute w1_mask
    PackedZ2<N_COLS> K_mask_x2;
    K_mask_x2.toeplitzByVec(K_mask,x2);

    PackedZ2<N_COLS> rK2_x_mask;
    rK2_x_mask.toeplitzByVec(rK2,x_mask);

    w2_mask = K_mask_x2;
    w2_mask ^= rK2_x_mask;
    w2_mask ^= sw2;

#ifdef PRINT_VAL
    std::cout<<"newprotocol.cpp/party2_round2(): "<<std::endl;
    std::cout<<"K_mask * x2"<<K_mask_x2<<std::endl;
    std::cout<<"rK2 * x_mask"<<rK2_x_mask<<std::endl;
    std::cout<<"w2_mask "<<w2_mask<<"\n"<<std::endl;
#endif
}

void compute_wmask(PackedZ2<N_COLS>& w_mask, PackedZ2<N_COLS>& w1_mask, PackedZ2<N_COLS>& w2_mask)
{
    w_mask = w1_mask;
    w_mask ^= w2_mask;
    #ifdef PRINT_VAL
        std::cout<<"newprotocol.cpp/compute_wmask(): "<<std::endl;
        std::cout<<"w_mask "<<w_mask<<std::endl;
    #endif
}

void party1_round3(PackedZ3<81>& y1_z3,PackedZ3<N_SIZE>& r0z1,
                   PackedZ3<N_SIZE>& r1z1, std::vector<PackedZ3<81> >& Rmat,PackedZ2<N_COLS>& w_mask)
{
    //PARTY 1: calculates the value of mux(w', r0z1, r1z1)
    PackedZ3<N_SIZE> res1;
    res1 = r0z1; //copy the contents of r0z to res1.

    //perform the mux functionality, pass the Packedz3 and converted vector of w_mask
    res1.mux(r1z1, w_mask.bits); //w_mask selects r0z if it is 0 else r1z
    y1_z3.matByVec(Rmat, res1); //party 1 computes y1 =  M * res1

    #ifdef UNIT_NP
        res1_global = res1;
    #endif

#ifdef PRINT_VAL //prints res1 and y1
    std::cout<<"\n newprotocol.cpp/party1_round3(): "<<std::endl;
    std::cout<<"res1 "<<res1<<std::endl;
    std::cout<<"y1_z3 "<<y1_z3<<std::endl;
#endif
}

void party2_round3(PackedZ3<81>& y2_z3,PackedZ3<N_SIZE>& r0z2,
                   PackedZ3<N_SIZE>& r1z2, std::vector<PackedZ3<81> >& Rmat,PackedZ2<N_COLS>& w_mask)
{
    //party 2 calculates the value of mux(w', r0z, r1z)
    PackedZ3<N_SIZE> res2;
    res2 = r0z2;

    //perform the mux functionality, pass the Packedz3 and converted vector of w_mask
    res2.mux(r1z2, w_mask.bits); //w_mask selects r0z if it is 0 else r1z
    y2_z3.matByVec(Rmat, res2); //party 2 computes y2 = M* res2

    #ifdef UNIT_NP
        res2_global = res2;
    #endif

#ifdef PRINT_VAL
    std::cout<<"newprotocol.cpp/party2_round3(): "<<std::endl;
    std::cout<<"res1 "<<res2<<std::endl;
    std::cout<<"y2_z3 "<<y2_z3<<std::endl;
#endif
}

//====================================Round 3 lookup implementation=======NEEDS DEBUGGING=========
#ifdef TEST_NP_LOOKUP
void party1_round3_lookup(PackedZ3<81>& y1_z3,PackedZ3<N_SIZE>& r0z1,
                          PackedZ3<N_SIZE>& r1z1, std::vector<PackedZ3<81> >& Rmat,PackedZ2<N_COLS>& w_mask)
{
#ifdef PRINT_VAL
    std::cout<<"newprotocol.cpp/party1_round3_lookup: inside the function"<<std::endl;
#endif
    PackedZ3<81> result_sum_lsb;
    PackedZ3<81> result_sum_msb;
    std::vector<uint64_t> lsb_input;
    lsb_input.resize(16);
    std::vector<uint64_t> msb_input;
    msb_input.resize(16);
    PackedZ2<256> temp_res_msb;
    PackedZ2<256> temp_res_lsb;

    std::chrono::time_point<std::chrono::system_clock> start_r3_mux = std::chrono::system_clock::now();
    //PARTY 1: calculates the value of mux(w', r0z1, r1z1)
    PackedZ3<N_SIZE> res1;
    res1 = r0z1; //copy the contents of r0z to res1.
    res1.mux(r1z1, w_mask.bits); //w_mask selects r0z if it is 0 else r1z

    timer_round3_mux += (std::chrono::system_clock::now() - start_r3_mux).count();

    temp_res_lsb = res1.lsbs();
    temp_res_msb = res1.msbs();
    reformat_input(lsb_input,temp_res_lsb);
    reformat_input(msb_input,temp_res_msb);

#ifdef PRINT_VAL
    std::cout<<"lsb_input(party 1): "<<lsb_input<<std::endl;
    std::cout<<"msb_input(party 1): "<<msb_input<<std::endl;
#endif
    std::chrono::time_point<std::chrono::system_clock> start_r3_lookup = std::chrono::system_clock::now();
    //call the use lookup table
    uselookup(result_sum_lsb,lsb_input,lookup_table);
    uselookup(result_sum_msb,msb_input,lookup_table);
    //perform subtraction
    y1_z3 = result_sum_lsb;
    y1_z3.subtract(result_sum_msb);
    timer_round3_lookup += (std::chrono::system_clock::now() - start_r3_lookup).count();

    timer_round3 += timer_round3_mux;
    timer_round3 += timer_round3_lookup;
}
void party2_round3_lookup(PackedZ3<81>& y2_z3,PackedZ3<N_SIZE>& r0z2,
                          PackedZ3<N_SIZE>& r1z2, std::vector<PackedZ3<81> >& Rmat,PackedZ2<N_COLS>& w_mask)
{
    PackedZ3<81> result_sum_lsb;
    PackedZ3<81> result_sum_msb;
    std::vector<uint64_t> lsb_input;
    std::vector<uint64_t> msb_input;

    lsb_input.resize(16);
    msb_input.resize(16);

    std::chrono::time_point<std::chrono::system_clock> start_r3_mux = std::chrono::system_clock::now();
    //party 2 calculates the value of mux(w', r0z, r1z)
    PackedZ3<N_SIZE> res2;
    res2 = r0z2;
    res2.mux(r1z2, w_mask.bits); //w_mask selects r0z if it is 0 else r1z
    timer_round3_mux += (std::chrono::system_clock::now() - start_r3_mux).count();

    reformat_input(lsb_input,res2.lsbs());
    reformat_input(msb_input,res2.msbs());
#ifdef PRINT_VAL
    std::cout<<"lsb_input(party 2): "<<lsb_input<<std::endl;
    std::cout<<"msb_input(party 2): "<<msb_input<<std::endl;
#endif
    std::chrono::time_point<std::chrono::system_clock> start_r3_lookup = std::chrono::system_clock::now();
    //call the use lookup table
    uselookup(result_sum_lsb,lsb_input,lookup_table);
    uselookup(result_sum_msb,msb_input,lookup_table);
    //perform subtraction
    y2_z3 = result_sum_lsb;
    y2_z3.subtract(result_sum_msb);

    timer_round3_lookup += (std::chrono::system_clock::now() - start_r3_lookup).count();

    timer_round3 += timer_round3_mux;
    timer_round3 += timer_round3_lookup;
}

#endif
//================================================================================


void display_exec_timing()  //displays the timing for each round, each party in that round and entire PRF
{
    using Clock = std::chrono::system_clock;
    using Duration = Clock::duration;
    //std::cout << Duration::period::num << " , " << Duration::period::den << '\n';
    float time_unit_multiplier = 1;
    if(Duration::period::den == 1000000000)
        time_unit_multiplier = 0.001; //make nanosecond to microsecond
    else if(Duration::period::den == 1000000)
        time_unit_multiplier = 1;   //keep the unit as microsecond

    timer_round1 = std::max(timer_round1_p1, timer_round1_p2) + timer_round1_mask;
    timer_round2 = std::max(timer_round2_p1, timer_round2_p2) + timer_round2_mask;
    timer_round3 = std::max(timer_round3_p1, timer_round3_p2);
    timer_np_prf = timer_round1 + timer_round2 + timer_round3;

    std::cout<<"Time to execute phase 1(both party runs simultaneously): "<<(timer_round1 * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"Time to execute phase 2(both party runs simultaneously): "<<(timer_round2 * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"Time to execute phase 3(both party runs simultaneously): "<<(timer_round3 * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"============================================================"<<std::endl;
    std::cout<<"Number of rounds per second for phase 1 "<<(1000/(timer_round1*time_unit_multiplier)*1000000)<<std::endl;
    std::cout<<"Number of rounds per second for phase 2 "<<(1000/(timer_round2*time_unit_multiplier)*1000000)<<std::endl;
    std::cout<<"Number of rounds per second for phase 3 "<<(1000/(timer_round3*time_unit_multiplier)*1000000)<<std::endl;
    std::cout<<"Number of rounds per second for entire PRF "<<(1000/(timer_np_prf*time_unit_multiplier)*1000000)<<std::endl;
    std::cout<<"=========================Breaking down the timings by parties for each round==================================="<<std::endl;
    std::cout<<"Time to execute phase 1 party 1: "<<(timer_round1_p1 * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"Time to execute phase 1 party 2: "<<(timer_round1_p2 * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"Time to execute phase 1 mask: "<<(timer_round1_mask * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"Time to execute phase 2 party 1: "<<(timer_round2_p1 * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"Time to execute phase 2 party 2: "<<(timer_round2_p2 * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"Time to execute phase 2 mask: "<<(timer_round2_mask * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"Time to execute phase 3 party 1: "<<(timer_round3_p1 * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"Time to execute phase 3 party 2: "<<(timer_round3_p2 * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"=========================TOTAL TIME==================================="<<std::endl;
    std::cout<<"Time to execute entire new protocol PRF: "<<(timer_np_prf * time_unit_multiplier)<<" microseconds"<<std::endl;

}

/*
 * This function generates the variables and perform computation to simulate centralized
 * version of the new protocol
 */
void PRF_new_protocol(std::vector<uint64_t>& K1, PackedZ2<N_COLS>& x1,
                              std::vector<uint64_t>& K2, PackedZ2<N_COLS>& x2,
                              std::vector<PackedZ3<81> >& Rmat, PackedZ3<81>& y1_z3, PackedZ3<81>& y2_z3, unsigned int nRuns)
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
    preProc_mod2_dm2020(nRuns);//it was nRuns * 2 in previous protocol, needs to be changed later
    preProc_mod3_dm2020(nRuns);

    fetchPreproc_party1(rx1,rw,sw1,rK1,r0z1,r1z1);
    fetchPreproc_party2(rx2,rw,sw2,rK2, r0z2,r1z2);

#ifdef TEST_NP_LOOKUP //reformat Rmat and create a lookup table only if LOOKUP implementation is required
    reformat_Rmat(Rmat16, Rmat); //convert (81 X 256) matrix to (16 x 16 X 81) matrix
    create_lookup_table(Rmat16, lookup_table);//(a lookup table of 16 X 2^16) is generated
#endif

    std::cout<<" Number of Runs: "<<nRuns<<std::endl;
    #ifdef PRINT_VAL
        std::cout<<"in PRF_new_protocol, rx1= "<<rx1<<std::endl;
        std::cout<<"PRF_new_protocol, rx2 "<<rx2<<std::endl;
    #endif

    std::chrono::time_point<std::chrono::system_clock> start_r1_p1;
    std::chrono::time_point<std::chrono::system_clock> start_r1_p2;
    std::chrono::time_point<std::chrono::system_clock> start_r1_mask;

    std::chrono::time_point<std::chrono::system_clock> start_r2_p1;
    std::chrono::time_point<std::chrono::system_clock> start_r2_p2;
    std::chrono::time_point<std::chrono::system_clock> start_r2_mask;

    std::chrono::time_point<std::chrono::system_clock> start_r3_p1;
    std::chrono::time_point<std::chrono::system_clock> start_r3_p2;

    PackedZ3<81> out;

    PackedZ3<81> y1_z3_dummy;//dummy value to ensure that each value is accessed; crucial for timing

    for(unsigned int i = 0; i< nRuns;i++)
    {
        //3. Parties locally compute [x'] = [x] + [rx] and [K'] = [K] + [rk]
        PackedZ2<N_COLS> x1_mask, x_mask, x2_mask;
        std::vector<uint64_t> K1_mask(toeplitzWords), K2_mask(toeplitzWords), K_mask(toeplitzWords);

        start_r1_p1 = std::chrono::system_clock::now();
        party1_round_1(x1_mask,K1_mask,x1,rx1,K1,rK1);
        timer_round1_p1 += (std::chrono::system_clock::now() - start_r1_p1).count();

        start_r1_p2 = std::chrono::system_clock::now();
        party2_round_1(x2_mask,K2_mask,x2,rx2,K2,rK2);
        timer_round1_p2 += (std::chrono::system_clock::now() - start_r1_p2).count();

        start_r1_mask = std::chrono::system_clock::now();
        //both the parties are supposed to exchange their mask values
        compute_input_mask(x_mask, K_mask, x1_mask, x2_mask, K1_mask, K2_mask);
        timer_round1_mask += (std::chrono::system_clock::now() - start_r1_mask).count();


#ifdef PRINT_VAL
        std::cout<<"newprotocol.cpp/PRF_new_protocol_central(): Round 1 ends"<<std::endl;
    std::cout<<"newprotocol.cpp/PRF_new_protocol_central(): Round 2 begins"<<std::endl;
#endif

        //Round 2: Both parties compute w' = [k'x'] - k'[rx] - rk.x' + [rk * rk + rw]
        PackedZ2<N_COLS> w1_mask, w2_mask, w_mask; //w' = K'(x' - rx) - rK'*x' + sw

        start_r2_p1 = std::chrono::system_clock::now();
        party1_round2(w1_mask, K_mask,x_mask, rx1, rK1, x1, sw1);
        timer_round2_p1 += (std::chrono::system_clock::now() - start_r2_p1).count();

        start_r2_p2 = std::chrono::system_clock::now();
        party2_round2(w2_mask, K_mask,x_mask, rx2, rK2, x2, sw2);
        timer_round2_p2 += (std::chrono::system_clock::now() - start_r2_p2).count();

        start_r2_mask = std::chrono::system_clock::now();
        compute_wmask(w_mask, w1_mask, w2_mask);
        timer_round2_mask += (std::chrono::system_clock::now() - start_r2_mask).count();

#ifdef PRINT_VAL
        std::cout<<"newprotocol.cpp/PRF_new_protocol_central(): Round 2 ends"<<std::endl;
        std::cout<<"newprotocol.cpp/PRF_new_protocol_central(): Round 3 begins"<<std::endl;
#endif

#ifndef TEST_NP_LOOKUP //if TEST_NP_Lookup is not enabled run the matbyVec version
        start_r3_p1 = std::chrono::system_clock::now();
        party1_round3(y1_z3,r0z1,r1z1,Rmat,w_mask);
        timer_round3_p1 += (std::chrono::system_clock::now() - start_r3_p1).count();

        start_r3_p2 = std::chrono::system_clock::now();
        party2_round3(y2_z3,r0z2,r1z2,Rmat,w_mask);
        timer_round3_p2 += (std::chrono::system_clock::now() - start_r3_p2).count();
#endif

#ifdef TEST_NP_LOOKUP// if TEST_NP is enabled run the new protocol
        start_r3_p1 = std::chrono::system_clock::now();
        party1_round3_lookup(y1_z3,r0z1,r1z1,Rmat,w_mask);
        timer_round3_p1 += (std::chrono::system_clock::now() - start_r3_p1).count();

        start_r3_p2 = std::chrono::system_clock::now();
        party2_round3_lookup(y2_z3,r0z2,r1z2,Rmat,w_mask);
        timer_round3_p2 += (std::chrono::system_clock::now() - start_r3_p2).count();
#endif

        y1_z3_dummy += y1_z3;
        y1_z3_dummy+= y2_z3;


#ifdef PRINT_VAL
        std::cout<<"newprotocol.cpp/PRF_new_protocol_central(): Round 3 ends"<<std::endl;
#endif

#ifdef UNIT_NP
        std::cout<<"newprotocol.cpp/PRF_new_protocol_central(): NP_TEST is enabled; calling the test function"<<std::endl;

    //call the test round 3 unit test function and pass K, X, res1, res2
    //merge K1, K2 into K and x1, x2 into x
    PackedZ2<N_COLS> X = x1; //declare a variable
    X ^= x2;    //x = x1 + x2
    std::vector<uint64_t> K(toeplitzWords);
    for (int i = 0; i < K.size(); i++)
    {
        K[i] = K1[i] ^ K2[i];
    }

    std::cout<<"K "<<K<<std::endl;
    std::cout<<"X "<<X<<std::endl;
    std::cout<<"rw "<<rw_global<<std::endl;
    std::cout<<"w_mask "<<w_mask<<std::endl;

    test_round1(X, rx_global, x_mask);
    test_round2(K, X, rw_global, w_mask);
    test_round3(K,X,res1_global,res2_global);

#endif
    } //end of for loop, nTimes

    std::cout << "in PRF_new_protocol, y1_z3_dummy=" << y1_z3_dummy <<  std:: endl;

    //print the execution timings
    display_exec_timing();
}

