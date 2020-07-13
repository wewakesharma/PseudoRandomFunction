#include <iostream>
#include <random>
#include <bitset>
#include <cmath>
#include <chrono> 
#include <ctime> 
#include <cstdlib>

#include "dmweakPRF.h"
#include "dmweakPRFnoPack.h"

//unsigned long int z_final[4];//to store the final product values

using namespace std;


/*
 * compute the multiplication of the key with the input
 * Each word holds 64 packed binary bits
 * //the non-packed version - To write
 */
void VecMatMultnotPack(uint64_t key[256][256], uint64_t input[256], uint64_t z_final[256])
{
    for(int i=0; i < 256; i++) {
        z_final[i] = 0; // initialize the accumulator to zero
    }

    for (int k = 0; k < 256; k++)
        for(int jCol = 0; jCol < 256; jCol++)
		{
                uint64_t z = key[k][jCol] & input[jCol] ;
                z_final[k] ^= z;

		}
}

void VecMatMultnotPack2(uint64_t key[128][256], uint64_t input[256], uint64_t z_final[256])
{
    for(int i=0; i < 128; i++) {
        z_final[i] = 0; // initialize the accumulator to zero
    }

    for (int k = 0; k < 256; k++)
        for(int jCol = 0; jCol < 128; jCol++)
        {
            uint64_t z = key[k][jCol] & input[jCol] ;
            z_final[k] ^= z;

        }
}
//=================The functions defined below will be used in unpacked pi protocol evaluation=========

//Function to convert 4 words into 256 bits
void unpack(uint64_t (&packword)[4], uint64_t (&unpck_bits)[256])
{
    int pack_temp_out[256];
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 64; j++)
        {
            unpck_bits[64*i+j] = ((packword[i] >> j) & 1); //4 packed int are converted to 256 z2 bits.
        }
    }
}


//Function to multiply matrix and vector and output a 256 bit resultant vector.
void mat_vec_mult(uint64_t (&input)[4], uint64_t (&key)[4][256], uint64_t (&naive_out_p1)[256])
{
    //create a key matrix
    //do bit by bit calculation
    uint64_t keyMatrix[256][256];
    uint64_t inputVector [256];

    //Unpack the 4 word input into 256 bit inputVector
    for (int iRow = 0; iRow< 256; iRow++)//taking each column at a time, iCol seems more apt for this
    {
        uint64_t iWord = floor(iRow/wLen);//Current word
        uint64_t iBit = iRow%wLen;  //takes value from 0 to 63
        uint64_t inputWord = input[iWord];  //takes the whole word as the input
        uint64_t tempVar = inputWord >> (iBit); //Shift the input by value 0 to 63.
        inputVector[iRow] = (inputWord >> (iBit)) & 1; //Stores the value to inputVector
    }

    //unpack the 4x256 key into 256 x 256 bit key matrix
    for (int iRow = 0; iRow< 256; iRow++)
    {
        int iWord = floor(iRow / wLen);
        for (int iCol = 0; iCol < 256; iCol++)
        {
            uint64_t key_word = key[iWord][iCol];
            keyMatrix[iRow][iCol] = (key_word >> (iRow % wLen)) & 1; //shift it by 0-63 value and take the last bit.
        }
    }

    //computing the product of matrix and vector
    for (int iRow = 0; iRow< 256; iRow++)
    {
        int row_total = 0;
        for (int iCol = 0; iCol < 256; iCol++)
        {
            row_total += keyMatrix[iRow][iCol] * inputVector[iCol];
        }
        naive_out_p1[iRow] = (row_total%2);
    }
}

//256 X 256 matrix multiplied by 256 bit vector.
void unpacked_mat_vec_mult(uint64_t unpck_mat[256][256], uint64_t unpck_vec[256], uint64_t unpck_mult_result [256])
{
    uint64_t element_value;
    uint64_t col_sum;
    for(int iRow = 0; iRow < 256; iRow++)
    {
        col_sum = 0;
        for(int iCol = 0; iCol < 256; iCol++)
        {
            col_sum += unpck_mat[iRow][iCol] * unpck_vec[iCol];
        }
        unpck_mult_result[iRow] = col_sum;
    }
}


