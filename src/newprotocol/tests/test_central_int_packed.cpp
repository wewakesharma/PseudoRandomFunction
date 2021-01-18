//
// Created by Vivek Sharma on 1/18/21.
//

#include <iostream>
#include <cstdlib>
#include "packedMod2.hpp"
#include "packedMod3.hpp"
#include "mains.hpp"
//#include "PRF.hpp"
#include "packed_PRF_central.h"
#include "OT.hpp"
#include "Timing.hpp"
#include <chrono>
#include <darkmatter/integer_pack.h>
#include "lookup_functions.h"

#ifdef TEST_INTEGER_PACKING

int main()  {

    //long timer_PRF_packed = 0; //time the entire PRF computation nRuns times using lookup table
    //long timer_PRF_packed_p1 = 0;
    //long timer_PRF_packed_p2 = 0;
    //long timer_PRF_packed_p3 = 0;


    PackedZ3<81> outZ3_dummy;

    int nRuns = 1000; //number of times the program runs

    std::cout<<"Running Packed PRF Centralized with integer packing for "<<nRuns<<" times."<<std::endl;
    /*
     * Code to determine the high point time precision of the machine
     */
    using Clock = std::chrono::system_clock;
    using Duration = Clock::duration;
    std::cout << Duration::period::num << " ==> " << Duration::period::den << '\n';

    //generate the inputs
    std::vector<uint64_t> K1(toeplitzWords), K2(toeplitzWords);//key shares of parties
    PackedZ2<N_COLS> x1, x2; //input shares of parties
    PackedZ3<81> outZ3;//output shares of both the parties

    //generate random inputs
    randomWord(1); // use seed=1
    for (auto &w : K1) w = randomWord();
    K1[K1.size() - 1] &= topelitzMask; // turn off extra bits at the end
    for (auto &w : K2) w = randomWord();
    K2[K2.size() - 1] &= topelitzMask; // turn off extra bits at the end
    std::vector<uint64_t> K(toeplitzWords);
    for (int i = 0; i < K1.size(); i++)
    {
        K[i] = K1[i] ^ K2[i];
    }

    x1.randomize();
    x2.randomize();
    PackedZ2<N_COLS> X = x1; //declare a variable
    X ^= x2;

    //generate a 81 X 256 randomization matrix in Z3.
    std::vector<PackedZ3<81> > Rmat(256);
    for (auto &col : Rmat) // iterate over the columns
        col.randomize();

    //reformating Rmat from 81 X 256 to 14 X 256
    std::vector<std::vector<uint64_t> > Rmat14(256);//outer vector is of size 256 and inner is of size 14

    //pack the Rmat
    pack_matrix(Rmat14,Rmat);

    for(int i =0;i<256;i++)
    {
        std::cout<<Rmat14[i];
    }

    PackedZ2<N_COLS> outKX;
    outKX.toeplitzByVec(K,X);//perform K*X

    //perform matByVec and get the output.[K*x*Rmat]
    matByVec_int_pack(outZ3,  outKX,  Rmat14);

    std::cout<<"Output is "<<outZ3<<std::endl;
/*
    //call the lookup table generator
    std::cout<<"Generating lookup table of size (16 X 2^16)"<<std::endl;
    std::vector<std::vector<PackedZ3<81> > > lookup_table(16); //table has 16 rows and 65536 columns and each element is PackedZ3
    create_lookup_table(Rmat16,lookup_table);
    std::cout<<"Generation of lookup table.....COMPLETE"<<std::endl;




        PackedZ2<N_COLS> outKX;

        auto start_prf_lookup = std::chrono::system_clock::now();

        outKX.toeplitzByVec(K,X);   //K * X

        //converting the input format
        std::vector<uint64_t> outKX_input(16); //16 vectors each as a word of size 16 bits. Total containing 256 bits.
        reformat_input(outKX_input, outKX);  //phase 2

        PackedZ3<81> outZ3_lookup;

        auto start_prf_p3 = std::chrono::system_clock::now();
        uselookup(outZ3_lookup, outKX_input, lookup_table);
        timer_packed_cent_p3 += (std::chrono::system_clock::now() - start_prf_p3).count();
        timer_packed_PRF_lookup += (std::chrono::system_clock::now() - start_prf_lookup).count();

        //std::cout << "in test_packed_PRF_central_lookup, main function, " << "outZ3_lookup=" << outZ3_lookup << std::endl;
        outZ3_dummy += outZ3_lookup;
*/
        /*
    using Clock = std::chrono::system_clock;
    using Duration = Clock::duration;
    //std::cout << Duration::period::num << " , " << Duration::period::den << '\n';
    float time_unit_multiplier = 1;
    if(Duration::period::den == 1000000000)
        time_unit_multiplier = 0.001; //make nanosecond to microsecond
    else if(Duration::period::den == 1000000)
        time_unit_multiplier = 1;   //keep the unit as microsecond
    std::cout<<"Time for "<<nRuns<<" runs of calling/accessing lookup table "<<(timer_packed_cent_p3 * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"Time for "<<nRuns<<" runs of entire PRF using lookup table "<<(timer_packed_PRF_lookup * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"Number of rounds per second for accessing lookup table "<<(1000/(timer_packed_cent_p3*time_unit_multiplier)*1000000)<<std::endl;
    std::cout<<"Number of rounds per second for running entire PRF using lookup table "<<(1000/(timer_packed_PRF_lookup*time_unit_multiplier)*1000000)<<std::endl;
         */
}
#endif
