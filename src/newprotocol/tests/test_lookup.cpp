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

/*
void reformat_input(std::vector<uint64_t>& outKX_input, PackedZ2<256>& outKX)//converts 256 bit input(PackedZ2) to 16 vector of 16 bits each
{
#ifdef LOOKUP_PRINT_VAL
    std::cout<<std::endl<<"Getting inside the loop"<<std::endl;
#endif
    for(int base_counter = 0; base_counter < 16; base_counter++)
    {
        uint64_t input_word = 0;
        for(int offset_counter = 0; offset_counter < 16; offset_counter++)
        {
            input_word |= ((outKX.at(16*base_counter+offset_counter)) << offset_counter);
        }
        outKX_input[base_counter] = input_word;
    }
}

void reformat_Rmat(std::vector<vector<PackedZ3<81> > >& Rmat16, std::vector<PackedZ3<81> >& Rmat)
{
    for(int outer_vector = 0; outer_vector < 16; outer_vector++)
    {
        Rmat16[outer_vector].resize(16);
        for(int inner_vector = 0; inner_vector < 16; inner_vector++)
        {
            Rmat16[outer_vector][inner_vector] = Rmat[16*outer_vector+inner_vector];
        }
    }
}


void uselookup(PackedZ3<81>& result_sum, std::vector<uint64_t>& outKX_input, std::vector<std::vector<PackedZ3<81> > >& lookup_table)
{
    std::vector<PackedZ3<81> > result_table(16); //stores the result of multiplication using lookup table


    for(int word_count = 0; word_count < 16; word_count++)
    {
        uint64_t value = outKX_input[word_count];
        result_table[word_count] = lookup_table[word_count][value];
    }

    //add up the 16 count of packedmod3 values
    result_sum = result_table[0];
    for(int count = 1; count < 16; count++)
    {
        result_sum += result_table[count];
    }
}*/


#ifdef LOOKUP_TEST
int main() {
    //declare the variables
    int nTimes = 1;
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

    //converting the input format
    std::vector<uint64_t> outKX_input(16); //16 vectors each as a word of size 16 bits. Total containing 256 bits.
    reformat_input(outKX_input, outKX);

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

    //call the lookup table generator
    std::vector<std::vector<PackedZ3<81> > > lookup_table(16); //table has 16 rows and 65536 columns and each element is PackedZ3
    create_lookup_table(Rmat16,lookup_table);

    PackedZ3<81> outZ3_lookup;
    uselookup(outZ3_lookup, outKX_input, lookup_table);


#ifdef LOOKUP_PRINT_VAL
    std::cout<<"The final value(lookup) is "<<outZ3_lookup<<std::endl;
    std::cout<<"The final value(matByVec) is "<<outZ3<<std::endl;
#endif

    if(outZ3 == outZ3_lookup)
        std::cout<<"Lookup table test status: Test passed"<<std::endl;
    else
        std::cout<<"Lookup table test status: Test failed"<<std::endl;
    return 0;
}

#endif