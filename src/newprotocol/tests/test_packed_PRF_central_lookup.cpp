//
// Created by Vivek Sharma on 10/28/20.
//

#include <iostream>
#include <cstdlib>
#include "packedMod2.hpp"
#include "packedMod3.hpp"
#include "mains.hpp"
#include "PRF.hpp"
#include "packed_PRF_central.h"
#include "OT.hpp"
#include "Timing.hpp"
#include <chrono>
#include "lookup_functions.h"

#ifdef PACKED_PRF_CENTRAL_LOOKUP


void set_input(std::vector<uint64_t>& K, PackedZ2<N_COLS>& x, std::vector< PackedZ3<81> >& Rmat)
{
    randomWord(1); // use seed=1

    //randomize the matrix
    for (auto &col : Rmat) // iterate over the columns
        col.randomize();

    // Choose random K,x
    for (auto &w : K) w = randomWord();
    K[K.size() - 1] &= topelitzMask; // turn off extra bits at the end

    x.randomize();     //randomize input
}

int main()  {

    long timer_packed_cent_p3 = 0;//time third phase
    long timer_packed_PRF_lookup = 0; //time the entire PRF computation nRuns times using lookup table

    PackedZ3<81> outZ3_dummy;//dummy output for accurate timing

    int nRuns = 1000; //number of times the program runs

    std::cout<<"Running Packed PRF Centralized with LOOKUP tables for "<<nRuns<<" times."<<std::endl;
    /*
     * Code to determine the high point time precision of the machine
     */
    using Clock = std::chrono::system_clock;
    using Duration = Clock::duration;
    std::cout << Duration::period::num << " ==> " << Duration::period::den << '\n';

    //generate the inputs
    std::vector<uint64_t> K(toeplitzWords);//key shares of parties
    PackedZ2<N_COLS> X; //input shares of parties
    std::vector<PackedZ3<81> > Rmat(256);

    //variables defined for lookup implementation
    std::vector<std::vector<PackedZ3<81> > > Rmat16(16);//16 X (81 X 16) matrix to construct lookup table
    std::vector<std::vector<PackedZ3<81> > > lookup_table(16); //table has 16 rows and 65536 columns and each element is PackedZ3

    PackedZ3<81> outZ3;//output shares of both the parties

    //generate random inputs
    set_input(K,X,Rmat);//set random values in K, X and Rmat

    //converting the Rmat format
    reformat_Rmat(Rmat16, Rmat);//convert 81 X 256 to 16 X (81 X 16) matrix, latter makes easier to construct lookup table

    //call the lookup table generator
    std::cout<<"Generating lookup table of size (16 X 2^16)"<<std::endl;

    create_lookup_table(Rmat16,lookup_table);   //creates lookup table 16 X 65535
    std::cout<<"Generation of lookup table.....COMPLETE"<<std::endl;

    for(int run_count = 0; run_count < nRuns; run_count++)//1000 runs
    {
        PackedZ2<N_COLS> outKX;//stores the Z2 output of Kx

        auto start_prf_lookup = std::chrono::system_clock::now();//start timing for entire PRF with lookup table

        outKX.toeplitzByVec(K,X);   //K * X in mod 2 field

        //converting the input format
        std::vector<uint64_t> outKX_input(16); //16 vectors each as a word of size 16 bits. Total containing 256 bits.
        reformat_input(outKX_input, outKX);  //phase 2

        PackedZ3<81> outZ3_lookup;//stores output after accessing lookup table

        auto start_prf_p3 = std::chrono::system_clock::now();

        uselookup(outZ3_lookup, outKX_input, lookup_table);//use of lookup table

        timer_packed_cent_p3 += (std::chrono::system_clock::now() - start_prf_p3).count();

        timer_packed_PRF_lookup += (std::chrono::system_clock::now() - start_prf_lookup).count();

        outZ3_dummy += outZ3_lookup;//dummy variable to ensure accurate timing in multiple runs
    }

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
}
#endif