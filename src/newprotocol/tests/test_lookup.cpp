//
// Created by Vivek Sharma on 10/21/20.
//

/*
 * Include header files required in this program - Unit testing of lookup table
 */
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

//using namespace std;

#ifdef UNIT_LOOKUP
int main() {
    //declare the variables
    int nTimes = 1000;
    long timer_reformat_input = 0;
    long timer_use_lookup = 0;
    std::vector<uint64_t> K1(toeplitzWords), K2(toeplitzWords);//key shares of parties
    PackedZ2<N_COLS> x1, x2; //input shares of parties
    PackedZ3<81> outZ3;//output shares of both the parties

    //generate random inputs
    randomWord(1); // use seed=1
    for (auto &w : K1) w = randomWord();
    K1[K1.size() - 1] &= topelitzMask; // turn off extra bits at the end
    for (auto &w : K2) w = randomWord();
    K2[K2.size() - 1] &= topelitzMask; // turn off extra bits at the end
    x1.randomize();
    x2.randomize();

    //generate a 81 X 256 randomization matrix in Z3.
    std::vector<PackedZ3<81> > Rmat(256);
    for (auto &col : Rmat) // iterate over the columns
        col.randomize();

    /*
     * compute outKX in Z3: NOTE: Instead of calling PRF packed centralized; we perform that function to
     * obtain intermediate value like "outKX" apart from usual "outZ3"
    */

    //1.perform X = x1+ x2 (on vectors)
    PackedZ2<N_COLS> X = x1; //declare a variable
    X ^= x2;

    //2.perform K = k1 + k2 (on matrix)
    std::vector<uint64_t> K(toeplitzWords);
    for (int i = 0; i < K1.size(); i++)
    {
        K[i] = K1[i] ^ K2[i];
    }

    PackedZ2<N_COLS> outKX;
    outKX.toeplitzByVec(K,X);
    std::vector<unsigned int> outKX_unsgn;//unsigned int of outKX i.e.(K*x)
    outKX.toArray(outKX_unsgn);

    PackedZ3<256> outKX_Z3;//intermediate value of (K*X) packed Z3
    outKX_Z3.fromArray(outKX_unsgn);//converting unsigned int to PackedZ2

    outZ3.matByVec(Rmat,outKX_Z3);//output of the centralized computation of PRF

#ifdef LOOKUP_PRINT_VAL
    for(int i = 0; i< 16; i++)
    {
        std::cout<<"The input is "<<std::endl;
        std::cout<<outKX_input[i]<<std::endl;
    }
#endif

    //converting the Rmat format
    std::vector<std::vector<PackedZ3<81> > > Rmat16(16);
    reformat_Rmat(Rmat16, Rmat);

    PackedZ3<81> outZ3_lookup;

    //call the lookup table generator
    std::vector<std::vector<PackedZ3<81> > > lookup_table(16); //table has 16 rows and 65536 columns and each element is PackedZ3
    create_lookup_table(Rmat16,lookup_table);

for(int i = 0; i< nTimes; i++)
{
    //converting the input format
    std::vector<uint64_t> outKX_input(16); //16 vectors each as a word of size 16 bits. Total containing 256 bits.
    auto start_reformat_input = std::chrono::system_clock::now();
    reformat_input(outKX_input, outKX);
    timer_reformat_input += (std::chrono::system_clock::now() - start_reformat_input).count();

    auto start_use_lookup = std::chrono::system_clock::now();
    uselookup(outZ3_lookup, outKX_input, lookup_table);
    timer_use_lookup += (std::chrono::system_clock::now() - start_use_lookup).count();
}


#ifdef LOOKUP_PRINT_VAL
    std::cout<<"The final value(lookup) is "<<outZ3_lookup<<std::endl;
    std::cout<<"The final value(matByVec) is "<<outZ3<<std::endl;
#endif

    using Clock = std::chrono::system_clock;
    using Duration = Clock::duration;
    //std::cout << Duration::period::num << " , " << Duration::period::den << '\n';
    float time_unit_multiplier = 1;
    if(Duration::period::den == 1000000000)
        time_unit_multiplier = 0.001; //make nanosecond to microsecond
    else if(Duration::period::den == 1000000)
        time_unit_multiplier = 1;   //keep the unit as microsecond
    std::cout<<std::endl<<"reformat "<<timer_reformat_input*time_unit_multiplier<<std::endl;
    std::cout<<std::endl<<"use lookup "<<timer_use_lookup*time_unit_multiplier<<std::endl;
    if(outZ3 == outZ3_lookup)
        std::cout<<"Lookup table test status: Test passed"<<std::endl;
    else
        std::cout<<"Lookup table test status: Test failed"<<std::endl;
    return 0;

}

#endif