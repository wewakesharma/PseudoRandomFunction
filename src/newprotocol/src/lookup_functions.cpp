//
// Created by Vivek Sharma on 10/21/20.
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

using namespace std;

void reformat_input(std::vector<uint64_t>& outKX_input, PackedZ2<256>& outKX)//converts 256 bit input(PackedZ2) to 16 vector of 16 bits each
{
#ifdef LOOKUP_PRINT_VAL
    std::cout<<std::endl<<"Getting inside the loop"<<std::endl;
#endif
    for(int base_counter = 0; base_counter < 16; base_counter++)//base counter is used to index 16 vectors of the output
    {
        uint64_t input_word = 0;
        for(int offset_counter = 0; offset_counter < 16; offset_counter++)//offset is used along with base counter to index PackedZ2 input and also used in shifting operation
        {
            input_word |= ((outKX.at(16*base_counter+offset_counter)) << offset_counter);
        }
        outKX_input[base_counter] = input_word;//once the word is created by cumulative shifting of bits, it is stored in outKX_input
    }
}

/*
 * Rmat is Randomization matrix of size 81 X 256. Since to create a lookup table, we need to combine 16 bits as a single word.
 * The Rmat also needs to be in similar format. reformat_Rmat does the exact same thing. It converts a single 81 X 256 matrix in to
 * 16 matrices, each of size 16 X 81(which is Rmat16)
 */
void reformat_Rmat(std::vector<std::vector<PackedZ3<81> > >& Rmat16, std::vector<PackedZ3<81> >& Rmat)
{
    for(int outer_vector = 0; outer_vector < 16; outer_vector++)//counts the 16  matrices created in output
    {
        Rmat16[outer_vector].resize(16);//setting up the column size 16 for each of those 16 matrices.
        //inner_vector: use to point columns of each of those 16 matrices and also used with outer_vector to point to original input matrix columns
        for(int inner_vector = 0; inner_vector < 16; inner_vector++)
        {
            Rmat16[outer_vector][inner_vector] = Rmat[16*outer_vector+inner_vector];
        }
    }
}

/*
 * creates a table of size 16 X (2^16)-1, which consists of precomputed product of all possible integers that can be made with 16 bits
 * with possible 16 bit values.
 */
void create_lookup_table(std::vector<std::vector<PackedZ3<81> > >& Rmat16,
                         std::vector<std::vector<PackedZ3<81> > >& lookup_table)
{
    PackedZ3<81> temp_result_vec;
    int packed_z3_counter; //go through the packedMod3 bit by bit
    int matrix_internal_pointer;//once matrix is selected, this pointer traverses through row of each column

    for(int matrix_pointer = 0; matrix_pointer < 16; matrix_pointer++)//iterates over the matrices(0 to 15)
    {
        lookup_table[matrix_pointer].resize(65536); //This implies each row has 65536 columns.
        for(int offset_pointer = 0; offset_pointer < 65536; offset_pointer++)//goes over the columns of lookup table
        {
            packed_z3_counter = 0; //reset the counter that goes bit by bit ovre packedmodz3
            temp_result_vec.reset(); //reset the temporary 81 bit vector which will store the result
            while(packed_z3_counter < 81)
            {
                int sum = 0; //initialize
                matrix_internal_pointer = 15; //start with 15th row of a matrix
                while(matrix_internal_pointer >= 0)
                {
                    sum += ((Rmat16[matrix_pointer][matrix_internal_pointer].at(packed_z3_counter))
                            * ((offset_pointer>>matrix_internal_pointer) & 1)); //taking each bit of the 16 bit value(0-65535)
                    matrix_internal_pointer--; //go upward toward the first row
                }
                temp_result_vec.set(packed_z3_counter,(sum%3)); //set the value computed after the multiplication
                packed_z3_counter++;
            }
            lookup_table[matrix_pointer][offset_pointer] = temp_result_vec; //set the packedmod3 value as an entry in lookup table
        }
 //       cout << "in create_lookup_table, matrix_pointer=" << matrix_pointer << endl;
    }
 //   cout << "in create_lookup_table, after all 16 rounds" << endl;
}

/*
 * The final and main part of lookup implementation is use of lookup table
 * This function takes input and lookup table that contains all the input
 */
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
}
