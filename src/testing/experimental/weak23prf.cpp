//cpp file for the header file

#include <iostream>
#include <random>
#include <bitset>
#include <cmath>
#include <chrono> 
#include <ctime> 
#include <cstdlib>
#include "weak23prf.h"

int wLen = 64;


//unsigned long int z_final[4];//to store the final product values

using namespace std;

/*
 * Generate 4 words of random data into the input array.
 */
void generate_input(uint64_t *input, std::mt19937 generator, int size)
{
    for(int i = 0; i < size; i++)
    {
        *(input+i) = generator();
    }
}

void generate_rand_key(uint64_t (&key)[4][256], std::mt19937 generator)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 256; j++)
        {
            key[i][j] = generator();
        }
    }
}

void generate_rand_matrix(uint64_t (&randMat1)[2][256], uint64_t (&randMat2)[2][256], std::mt19937 &generator)
{
	uint64_t temp_mat1, temp_mat2, temp_var;
	int cnt = 0;
    //for each word of the column, as we have 81 columns, so we need two 64-bit words for each
    for (int i = 0; i < 2; i++) 
    {
        for (int j = 0; j < 256; j++) 
        {
            int nBitsGenerated = 0;
            temp_mat1 = 0;
            temp_mat2 = 0;
            while (nBitsGenerated < wLen)  //we need two words for each column in the two MSB and LSB matrices we are filling
            {
                uint64_t wGen = generator();
                //k is the index within the wGen, which holds a random 64 bit word
                for (int k = 0; k < wLen; k = k + 2) 
                {   
                    //examine each two bits and make sure they are not 11
                  	uint64_t bit1 = (wGen >> k) & 1;
                    uint64_t bit2 = (wGen >> (k + 1)) & 1;

                    //make sure we don't have 11 - this is a mod 2 matrix so we can only have 00, 01 or 10
                    if (!((bit1 == 1) & (bit2 == 1)))
                    {
                            //assign the next bits generated to their locations in the random matrices, the location is nBitsFound
                            temp_mat1 |= (bit1 << nBitsGenerated);
                            temp_mat2 |= (bit2 << nBitsGenerated);
                            nBitsGenerated++;
                            //if we reached the end of the random matrix word, we need to go to the next word. We will then generate a new random word to fill it
                            if (nBitsGenerated == wLen)
                            break;
                    }

                }
            }
            randMat1[i][j] = temp_mat1; //to remove element 3 to appear in the matrices
            randMat2[i][j] = temp_mat2;
        }
            
    }
    //Set all bits to zero after 17th bit
    uint64_t fetch_16 = 0x000000000001FFFF;
    for(int col_count=0; col_count < 256; col_count++)
    {
        randMat1[1][col_count] &= fetch_16;
        randMat2[1][col_count] &= fetch_16;
    }
}

/*
*Performs the computation of phase 1 by wordpacking. This method is described in /Documents/Wordpacking-Example(new).pdf
*/
void compute(uint64_t (&key)[4][256], uint64_t (&input)[4], uint64_t (&z_final)[4])
{
    for(int i=0; i < 4; i++) 
    {
        z_final[i] = 0; // initialize the accumulator to zero
    }

	for(int i=0; i < 4; i++)
	{
        uint64_t x = input[i];
		for(int j = 0; j < wLen; j++)
		{
            uint64_t y = -((x>>j) & 1);

            for (int k = 0; k < 4; k++) 
            {
                uint64_t z = key[k][j+wLen*i];
                z_final[k] ^= (z & y);
            }
		}
	}
}

/*
* TESTING - Performs naive vector matrix multiplication of phase 1 
*/
void mat_vec_mult(uint64_t (&input)[4], uint64_t (&key)[4][256], uint64_t (&out)[256])
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
        out[iRow] = (row_total%2);
    }
}

/*
* The function compares the output of unpacked version and output of naive implementation- phase 1 testing
*/
void phase1_test(uint64_t (&z_final)[4], uint64_t (&out)[256])
{
    bool flag = 0;// comparing out and pack_temp_out; 0 stands for success
    int pack_temp_out[256];
    int cnt=0;
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 64; j++)
        {
            pack_temp_out[64*i+j] = ((z_final[i] >> j) & 1); //4 packed int are converted to 256 z2 bits.
        }
    }
    for(int i=0;i<256;i++)
    {
        if(out[i] != pack_temp_out[i]) //If the bits don't match, break the loop. The function is not 100% correct  now since we have bit arrangement issues.
        {
            cout<<endl<<"The bit at position "<<i<<" doesn't match"<<endl;
            cout<<endl<<"!!!- Exiting the testing phase -!!!"<<endl;
            flag = 1;
            break;
        }
    }
    if(flag == 1)
        cout<<"The computational outputs using naive and wordpacking DID NOT match, the function is NOT correct ==========> ERROR"<<endl; //prints if the break statement is not executed
    else
        cout<<"Output of naive and word-packed version  in phase 1 MATCHED ==========> O.K."<<endl;
}

