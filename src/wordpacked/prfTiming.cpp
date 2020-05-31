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

/*
 * Generate 4 words of random data into the input array.
 */
void generate_input(uint64_t input[4], std::mt19937 &generator)
{
    //srand(time(NULL));
    for(int i = 0; i < 4; i++)
    {
        //Call the generator that generate a 64-but word each time
        input[i] = generator();
    }
}

/*
 * Generate the random key matrix, which is 256x256
 */
void generate_rand_key(uint64_t key[4][256], std::mt19937 &generator)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 256; j++)
        {
            key[i][j] = generator();
        }
    }
}

/*
 * Generate a Z_3 randomizing matrix which is 81X256. This will result in a 128-bit entropy.
 *
 */

/*
 * Generate two random matrices - one with the MSB's and one with the LSB's
 * since the resuoting matrix is 81X256, we need 2x256 words
 * The combination of the matrices are numbers in Z_3, so we need to ensure that we do not have the number 11
 * in any of the matching MSB's and LSB's
 *
 */
void generate_rand_matrix(uint64_t randMat1[2][256], uint64_t randMat2[2][256], std::mt19937 &generator)
{

    //for each word of the column, as we have 81 columns, so we need two 64-bit words for each
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 256; j++) {
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
}



void compute(uint64_t key[4][256], uint64_t input[4], uint64_t z_final[4])
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
                uint64_t z = key[k][j+wLen*i];
                z_final[k] ^= (z & y);
            }
		}
	}
}

void addMod3(uint64_t& outM, uint64_t& outL, uint64_t msb1, uint64_t lsb1, uint64_t msb2, uint64_t lsb2)
{

    outM = ((msb1 ^ msb2) & (~lsb1) & (~lsb2)) | ((~msb1) & (~msb2) & lsb1 & lsb2);
    outL = ((~msb1) & (~msb2) &  (lsb1 ^ lsb2)) | (msb1 & msb2 & (~lsb1) & (~lsb2));
}

void unpackOutput(uint64_t output[4], char p2output[256])
{
    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < wLen; i++) {
            p2output[i] = ((output[i] >> j) & 1);
        }
    }
}

/*
 * multiply modulo 3
 */
void multMod3(uint64_t outM[2], uint64_t outL[2], uint64_t msbs[2][256], uint64_t lsbs[2][256], uint64_t in[4])
{
    uint64_t msb[2], lsb[2];

    //go over the input bits, one by one

    for (int i1 = 0; i1 < 4; i1++)
        for (int i2 = 0; i2 < 64; i2++)
        {
            uint64_t bit = -((in[i1] >> i2 ) & 1 ); //the input bit, replicated either all=0 or all=1

            for (int j = 0; j < 2; j++) //each column has 81 rows, so need 2 64-bit words
            {
                msb[j] = msbs[j][64*i1+i2] & bit;
                lsb[j] = lsbs[j][64*i1+i2] & bit;  //multiply by current bit
                addMod3(outM[j],outL[j],outM[j],outL[j],msb[j],lsb[j]); //add mod 3 to acumulator
            }

        }

}

//Step 1: Generate random input and store it in input array.
//Step 2: Generate 1024 uint64 numbers and store it in 4X256 size.
//Step 3: compute() function will calculate the value of zi's
int main()
{
	uint64_t key[4][256];
	uint64_t randMat1[2][256], randMat2[2][256];
    uint64_t input[4];
    uint64_t outM[2];
    uint64_t outL[2];

    unsigned seed = 7;    // std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed); // mt19937 is a standard mersenne_twister_engine
	generate_rand_key(key, generator);

	//we generate two random matrices, one holds the first bit and one the second bit
	generate_rand_matrix(randMat1, randMat2, generator);
    generate_input(input,generator);

    uint64_t output[4];
    char p2output[256];

    chrono::time_point<std::chrono::system_clock> start = chrono::system_clock::now();

    for(int i=0;i<1000000;i++){
    
        compute(key,input, output); // matrix-vector multiply mod 2
        unpackOutput(output,p2output); // useless operation that should not be here
        // This is where the mod2->mod3 protocol should be
        multMod3(outM, outL, randMat1, randMat2, output); // matrix-vector multiply mod 3
    }

    chrono::duration<double> elapsed_seconds = chrono::system_clock::now() - start;
  
    cout<<endl<<"output msb,lsb is "<< outM << ',' << outL << endl;

    cout << "elapsed time for 1M runs:  " << elapsed_seconds.count() << "  s\n";

	return 0;
}
