#include <iostream>
#include <random>
#include <bitset>
#include <cmath>
#include <chrono> 
#include <ctime> 
#include <cstdlib>

#include "dmweakPRF.h"

//unsigned long int z_final[4];//to store the final product values

using namespace std;

int wLen = 64;

void generate_rand_packed_vector_4(uint64_t vec[4], std::mt19937 &generator)
{

    //srand(time(NULL));
    for(int i = 0; i < 4; i++)
    {
        //Call the generator that generate a 64-but word each time
        vec[i] = generator();
    }
}

void generate_rand_Z3_packed_Vec_4(uint64_t PackedVecm[4], uint64_t PackedVecl[4], uint64_t unpackedVec[256], std::mt19937 &generator)
{

    //for each word of the column, as we have 81 columns, so we need two 64-bit words for each
    for (int jCol = 0; jCol < 256; jCol++) {
        //go over the columns
            //fill the numbers for each row

            PackedVecm[jCol]=0;
            PackedVecl[jCol]=0;
            unpackedVec[jCol] = 0;

            int nBitsGenerated = 0;
            while (nBitsGenerated < wLen)  //we need two words for each column in the two MSB and LSB matrices we are filling
            {

                uint64_t wGen = generator();
                //k is the index within the wGen, which holds a random 64 bit word
                for (int k = 0; k < wLen; k = k + 2) {

                    //if we already have 81 rows, the rest of the items in this column are set to 0

                    //examine each two bits and make sure they are not 11
                    uint64_t bit1 = (wGen >> k) & 1;
                    uint64_t bit2 = (wGen >> (k + 1)) & 1;

                    //make sure we don't have 11 - this is a mod 2 matrix so we can only have 00, 01 or 10
                    if (!((bit1 == 1) & (bit2 == 1)))
                    {
                        //assign the next bits generated to their locations in the random matrices, the location is nBitsFound
                        PackedVecm[jCol] |= (bit1 << nBitsGenerated);
                        PackedVecl[jCol] |= (bit2 << nBitsGenerated);
                        unpackedVec[jCol]=(bit1<<1 | bit2);
                        nBitsGenerated++;
                        //if we reached the end of the random matrix word, we need to go to the next word. We will then generate a new random word to fill it
                        if (nBitsGenerated == wLen) {
                            break; //go to the next item
                        }
                    }
                }
            }



        }
}



void generate_rand_sqMat_256(uint64_t mat[256][256], std::mt19937 &generator)
{

    for(int j = 0; j < 256; j++)
        for(int i = 0; i < 256; i++)
            {
            //Call the generator that generate a 64-but word each time
                mat[i][j] = generator() & 1;
            }
}

void generate_rand_packed_sqMat_4(uint64_t mat[4][256], std::mt19937 &generator)
{

    for(int j = 0; j < 256; j++)
        for(int i = 0; i < 4; i++)
        {
            //Call the generator that generate a 64-but word each time
            mat[i][j] = generator();
        }
}
//
/*
 * Generate 4 words of random data into the input array.
 */
void generate_rand_vector_256(uint64_t vec[256], std::mt19937 &generator)
{

    //srand(time(NULL));
    for(int i = 0; i < 256; i++)
    {
        //Call the generator that generate a 64-but word each time
        vec[i] = generator() & 1;
    }
}

void wordPackedVecMatMult(uint64_t key[4][256], uint64_t input[4], uint64_t z_final[4])
{
    for(int i=0; i < 4; i++) {
        z_final[i] = 0; // initialize the accumulator to zero
    }

    for(int i=0; i < 4; i++)
    {
        uint64_t x = input[i];
        for(int j = 0; j < wLen; j++)
        {
            uint64_t y = -((x>>j) & 1);

            for (int k = 0; k < 4; k++) {
                uint64_t z = key[k][j+(wLen*i)];
                z_final[k] ^= (z & y);
            }
        }
    }
}
/*
 * generate a random packed vector
 */




