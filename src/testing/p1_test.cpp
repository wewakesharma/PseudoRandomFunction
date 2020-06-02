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
    
    //Displaying the bits of input in z2
    
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
void mat_vec_mult(uint64_t input[4], uint64_t key[4][256], int out[256])
{
    int column_total = 0;
    int cnt = 0;
	for(int col_count = 0; col_count < 256; col_count++) //we have to traverse 256 columns in key matrix
    {
        column_total = 0;
        for(int row_count = 0; row_count < 4; row_count++) //traverses through 4 rows of each column in both key and input
        {
            for(int word_count = 0; word_count < wLen; word_count++)//each bit of a word
            {
                column_total = column_total + (((input[row_count]>>word_count)&1) * ((key[row_count][col_count]>>word_count)&1));
            }
        }
        out[cnt] = (column_total%2);
        cnt++;
    }
}


void compute(uint64_t key[4][256], uint64_t input[4], uint64_t z_final[4], int out[256])//compute() is equivalent to matvecmultiplication
{
    int pack_temp_out[256];
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
    cout<<"We are in compute function"<<endl;
    /*for(int i=0;i<256;i++)
    {
        cout<<out[i];
    }*/
    int cnt=0;
    for(int i=0;i<4;i++)
    {
        for(int j=0;j<wLen;j++)
        {
            pack_temp_out[i] = ((z_final[i]>>j)&1);

        }

    }
    //comparing out and pack_temp_out
    for(int i=0;i<256;i++)
    {
        if(out[i] != pack_temp_out[i])
        {
            cout<<"Some problem";
            break;
        }
    }
    
    
}



//Step 1: Generate random input and store it in input array.
//Step 2: Generate 1024 uint64 numbers and store it in 4X256 size.
//Step 3: compute() function will calculate the value of zi's
int main()
{
	uint64_t key[4][256];
    uint64_t input[4];
    int unpacked_input[256];
    int out[256];  //variable used for naive testing, represents output of phase 1
    uint64_t output[4];

    unsigned seed = 7;    // std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed); // mt19937 is a standard mersenne_twister_engine
	generate_rand_key(key, generator);

	//we generate two random matrices, one holds the first bit and one the second bit
	//generate_rand_matrix(randMat1, randMat2, generator);
    generate_input(input,generator);
    mat_vec_mult(input, key, out);
    
    compute(key,input, output,out); // matrix-vector multiply mod 2
	return 0;
}
