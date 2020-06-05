#include <iostream>
#include <random>
#include <bitset>
#include <cmath>
#include <chrono> 
#include <ctime> 
#include <cstdlib>

int wLen = 64;

//unsigned long int z_final[4];//to store the final product values

using namespace std;

void generate_rand_matrix(uint64_t randMat1[2][256], uint64_t randMat2[2][256], std::mt19937 &generator)
{
    //for each word of the column, as we have 81 columns, so we need two 64-bit words for each
    for (int i = 0; i < 2; i++) 
    {
        for (int j = 0; j < 256; j++) 
        {
            int nBitsGenerated = 0;
            while (nBitsGenerated < wLen)  //we need two words for each column in the two MSB and LSB matrices we are filling
            {
                uint64_t wGen = generator();
                //k is the index within the wGen, which holds a random 64 bit word
                for (int k = 0; k < wLen; k = k + 2) {
                    //examine each two bits and make sure they are not 11
                    int bit1 = (wGen >> k) & 1;
                    int bit2 = (wGen >> (k + 1)) & 1;

                    //make sure we don't have 11 - this is a mod 2 matrix so we can only have 00, 01 or 10
                    if (~((bit1 == 1) & (bit2 == 1)))
                    {
                        //assign the next bits generated to their locations in the random matrices, the location is nBitsFound
                        randMat1[i][j] |= (bit1 << nBitsGenerated);
                        randMat2[i][j] |= (bit2 << nBitsGenerated);
                        nBitsGenerated++;
                        //if we reached the end of the random matrix word, we need to go to the next word. We will then generate a new random word to fill it
                        if (nBitsGenerated == wLen)
                            break;
                    }
                }
            }

        }
    }
    //Displaying randmat1 and randmat2
    for(int row =0; row < 2; row++)
    {
        for(int col = 0; col < 256; col++)
        {
            cout<<randMat1[row][col]<<"\t"<<randMat2[row][col]<<endl;
        }
        cout<<endl;
    }
}

int main()
{
    
    uint64_t key[4][256];
    uint64_t input[4];
    uint64_t output[4];
    int unpacked_input[256];
    int out[256];  //variable used for naive testing, represents output of phase 1
    char p2output[256];
    //========== Phase 2==================
    uint64_t randMat1[2][256], randMat2[2][256];
    uint64_t outM[2];
    uint64_t outL[2];
    int z3_mat[81][256];
    
    unsigned seed = 7;    
    std::mt19937 generator(seed); // mt19937 is a standard mersenne_twister_engine

    generate_rand_matrix(randMat1, randMat2, generator);

    return 0;
}