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

void create_lookup_table(std::vector<std::vector<PackedZ3<81> > >& Rmat16,
                         std::vector<std::vector<PackedZ3<81> > >& lookup_table)//create 16 count of (16 X 81) matrix
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
    }
}
