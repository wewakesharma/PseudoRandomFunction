//
// Created by Vivek Sharma on 1/18/21.
//

#include "integer_pack.h"
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
#include "integer_pack.h"

using namespace std;


//pack matrix of 81 X 256 into 14 X 256 of packed integer elements
void pack_matrix(std::vector<vector<uint64_t> >& Rmat14, std::vector<PackedZ3<81> >& Rmat)
{
    for(int col_count = 0; col_count < N_COLS; col_count++)
    {
        Rmat14[col_count].resize(14);//each column of Rmat will have 14 rows.
        int z3_value = 0;       //extracted z3 bit from Randomization matrix
        int word_count = 0;     //To be REMOVED
        int packed_value = 0;       //stores the final packed integer value
        for(int row_count = 0; row_count < 14; row_count++)
        {
            uint64_t temp_value;
            for(int inner_row = 0; inner_row < 6; inner_row++)
            {
                z3_value = Rmat[col_count].at(row_count*6+inner_row); //extract a z3 bit from Rmat
                packed_value &= z3_value<<((5-inner_row)*10);   //first bit will be pushed 50 to left, second pushed 40 to left so on
                if((row_count*inner_row) > 80)
                    break;
            }
            Rmat14[row_count][col_count] = packed_value;
        }

    }
}

void unpack_output(std::vector<uint64_t>& out_Z3, std::vector<uint64_t>& out_pack)//this function unpacks 14 words to 81 bits
{

}


void matByVec_int_pack(std::vector<uint64_t>& out_Z3, PackedZ2<N_COLS>& outKX,
        std::vector<vector<uint64_t> >& Rmat14)//performs the matrix vector product on the packed integers
{
    std::vector<uint64_t> out_pack; //packed output
    out_pack.resize(14);
    for(int i = 0; i < 14; i++)//number of words(rows)
    {
        uint64_t sum = 0;
        for(int j = 0; j < N_COLS; j++)//number of columns
        {
            sum += outKX.at(j) * Rmat14[i][j];
        }
        out_pack[i] = sum % 3;
    }
    unpack_output(out_Z3, out_pack);//internally calls the unpacking function

}


//