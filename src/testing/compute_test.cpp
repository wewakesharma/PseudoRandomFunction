#include <iostream>
#include <random>
#include <bitset>
#include <cmath>
#include <chrono> 
#include <ctime> 
#include <cstdlib>

//Questions:
//How do i compare the output from mat_vec_mult() and compute(), one gives 256 bit output and other a 4 word output

int wLen = 64;

//unsigned long int z_final[4];//to store the final product values

using namespace std;

/*
 * Generate 4 words of random data into the input array.
 */
void generate_input(uint64_t input[4], std::mt19937 &generator)
{
    for(int i = 0; i < 4; i++)
    {
        //Call the generator that generate a 64-bit word each time
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
void mat_vec_mult(uint64_t input[4], uint64_t key[4][256], uint64_t out[256])
{
	int count =0;
	//cout<<"The multiplication value is "<<endl;
    for(int j=0;j<256;j++)
    {
    	out[j]=0;
    	uint64_t temp;
    	int total_one_bit = 0;
    	uint64_t result=0;
    	for(int i=0; i<4;i++)
    	{
    		result = result | (input[i] & key[i][j]);//Taking product of ith input with key (i,j) and cumulative store in result
    		//std::bitset<64> x(res);
    		//cout<<res<<endl;
    		
    		//cout<<x.count()<<endl;
    	}
    	//cout<<result<<endl;
    	//count++;
    	out[j] = result;//save it in output array for 81 X 256 matrix multiplication.
    	cout<<out[j]<<endl;
    	//cout<<count;
    }
    //cout<<count;//print the size of output array
}
/*
 * Generate a Z_3 randomizing matrix which is 81X256. This will result in a 128-bit entropy.
 *
 */

/*
 * Generate two random matrices - one with the MSB's and one with the LSB's
 * since the resulting matrix is 81X256, we need 2x256 words
 *
 */
void generate_rand_matrix(uint64_t randMat1[2][256], uint64_t randMat2[2][256], std::mt19937 &generator)
{
    int rInt = 0;
    int nBitsFound=0;

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 256; j++) {
            nBitsFound = 0;
            while (nBitsFound < wLen)  //we need two words for each word in the two MSB and LSB matrices we are filling
            {
                uint64_t wGen = generator();
                //k is the index within the wGen
                for (int k = 0; k < wLen; k = k + 2) {
                    int bit1 = (wGen >> k) & 1;
                    int bit2 = (wGen >> (k + 1)) & 1;

                    //make sure we don't have 11 - this is a mod 2 matrix so we can only have 00, 01 or 10
                    if (~((bit1 == 1) & (bit2 == 1)))
                    {
                        randMat1[i][j] |= (bit1 << k);
                        randMat2[i][j] |= (bit2 << (k + 1));
                        nBitsFound++;

                    }
                }
            }

        }
    }

    //Diplaying the content of 
    cout<<"Displaying the content of random matrix"<<endl;
    for(int i=0;i<2;i++)
    {
        for(int j=0;j<256;j++)
        {
            cout<<randMat1[i][j]<<"\t"<<randMat2[i][j]<<endl;
        }
        cout<<endl;
    }
}

/*void phase3()
{

}*/

void compute(uint64_t key[4][256], uint64_t input[4], uint64_t z_final[4])//compute() is equivalent to matvecmultiplication
{
    for(int i=0; i < 4; i++) {
        z_final[i] = 0; // initialize the accumulator to zero
    }

	for(int i=0; i < 4; i++)//traverse through the input(4 words)
	{
        uint64_t x = input[i]; //takes each word of input
		for(int j = 0; j < wLen; j++)//traverse through each bit of each input
		{
            uint64_t y = -((x>>j) & 1);

            for (int k = 0; k < 4; k++) {
                uint64_t z = key[k][j+wLen*i];//Take each key from the key matrix. row by row
                z_final[k] ^= (z & y);//bitwise AND with the y.
            }
		}
	}
    for(int i =0;i<4;i++)
    {
        cout<<z_final[i];
    }
}

void addMod3(uint64_t& outM, uint64_t& outL, uint64_t msb1, uint64_t lsb1, uint64_t msb2, uint64_t lsb2)
{
    outL = ((~msb1) & (~msb2) &  (lsb1 ^ lsb2)) | (msb1 & msb2 & (~lsb1) & (~lsb2)); //covers, 00 additions
    outM ^= ((msb1 ^ msb2) & lsb1 & lsb2) | ((~msb1) & (~msb2) & lsb1 & lsb2);
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

    for (int i1 = 0; i1 < 4; i1++){
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

}

//Step 1: Generate random input and store it in input array.
//Step 2: Generate 1024 uint64 numbers and store it in 4X256 size.
//Step 3: compute() function will calculate the value of zi's
int main()
{
	uint64_t key[4][256];
	uint64_t randMat1[2][256], randMat2[2][256];
    uint64_t input[4];
    uint64_t out[256];//variable used for naive testing, represents output of phase 1
    uint64_t outM[2];
    uint64_t outL[2];

    unsigned seed = 7;    // std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed); // mt19937 is a standard mersenne_twister_engine
	generate_rand_key(key, generator);

	//we generate two random matrices, one holds the first bit and one the second bit
	generate_rand_matrix(randMat1, randMat2, generator);
    generate_input(input,generator);
    //mat_vec_mult(input, key, out);
    //uint64_t output[4];
    //char p2output[256];
    //compute(key,input, output); // matrix-vector multiply mod 2
    //unpackOutput(output,p2output); // useless operation that should not be here
    // This is where the mod2->mod3 protocol should be
    multMod3(outM, outL, randMat1, randMat2, output); // matrix-vector multiply mod 3

  
    cout<<endl<<"output msb,lsb is "<< outM << ',' << outL << endl;


	return 0;
}
