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
 * Generate 4 words of random data and store in an uint64_t input array.
 */
void generate_input(uint64_t input[4], std::mt19937 &generator)
{
    for(int i = 0; i < 4; i++)
    {
        input[i] = generator();//generating random word of integer
    }
}

void generate_debug_input(uint64_t input[4])
{
    input[0] = 1;
    for(int i = 1; i < 4; i++)
    {
        input[i] = 0; //debug input is only 1 bit per word
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
            key[i][j] = generator();//generate 4 x 256 key matrix
        }
    }
}

void generate_debug_key(uint64_t key[4][256])
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 256; j++)
        {
            key[i][j] = 255; // all the values in this key matrix is 255 which translates to 0x0000000001111111
        }
    }
}

//Testing code for phase 1 matrix vector multiplication using naive implementation.
void mat_vec_mult(uint64_t input[4], uint64_t key[4][256], int out[256])
{
    //create a key matrix
    //do bit by bit calculation

    uint64_t keyMatrix[256][256]; //temporary storage to unpack and store the packed key
    uint64_t inputVector [256]; //storage to unpack and store the rendomly generated input

    for (int iRow = 0; iRow< 256; iRow++)
    {
        uint64_t iWord = floor(iRow/wLen); //iWord is the section(word) of input we are in; can be int to save memory
        uint64_t iBit = iRow%wLen; //values from 0 to 63; can be an int to save memory
        uint64_t inputWord = input[iWord];  //choose the word of the input
        uint64_t tempVar = inputWord >> (iBit); //right shift input word by specific positions
        if (iRow==32)
            cout << tempVar << "\n";
        inputVector[iRow] = (inputWord >> (iBit)) & 1; //save the bits in it in input vector
    }


        for (int iRow = 0; iRow< 256; iRow++) {
            int iWord = floor(iRow / wLen);
            for (int iCol = 0; iCol < 256; iCol++) {
                uint64_t key_word = key[iWord][iCol];
                keyMatrix[iRow][iCol] = (key_word >> (iRow % wLen)) & 1;
            }
        }

    for (int iRow = 0; iRow< 256; iRow++) {
        int row_total = 0;
        for (int iCol = 0; iCol < 256; iCol++) {
            row_total += keyMatrix[iRow][iCol] * inputVector[iCol];
        }
        out[iRow] = (row_total%2);
    }
}


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
                uint64_t z = key[k][j+(wLen*i)]; //Take each key from the key matrix. row by row
                z_final[k] ^= (z & y);//bitwise AND with the y.
            }
		}
	}
}

void compute_debug(uint64_t key[4][256], uint64_t input[4], uint64_t z_final[4])//compute() is equivalent to matvecmultiplication
{
    for(int i=0; i < 4; i++) {
        z_final[i] = 0; // initialize the accumulator to zero
    }

    int i = 0;
    //for(int i=0; i < 4; i++)//traverse through the input(4 words)
    {
        uint64_t x = input[i]; //takes each word of input
        int j = 0; //only look at the first big of the each
     //   for(int j = 0; j < wLen; j++)//traverse through each bit of each input
        {
            uint64_t y = -((x>>j) & 1);

            for (int k = 0; k < 4; k++) {
                uint64_t z = key[k][j+(wLen*i)]; //Take each key from the key matrix. row by row
                z_final[k] ^= (z & y);//bitwise AND with the y.
                int temp = z_final[0];
                int firstbit = temp&1;
                int firstkeybit = key[0][0] & 1;
                cout << temp << " first bit" << firstbit << "\n";
            }
        }
    }
}

/*
* The function compares the output of unpacked version and output of naive implementation
*/
void final_test(uint64_t z_final[4], int out[256])
{
    int flag = 1; //the outputs do not match
	int pack_temp_out[256];
	int cnt=0;
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 64; j++)
        {
            pack_temp_out[64*i+j] = ((z_final[i] >> j) & 1); //4 packed int are converted to 256 z2 bits.
        }
    }
    int p_count = 0;
    int out_count = 0;
    for(int i = 0; i < 256; i++)
    {

    	cout<<pack_temp_out[i]<<"\t"<<out[i]<<endl;// printing the two outputs index by index
    	//**Apart from comparing out and pack_temp_out, we also count the number of set bits
    	if(pack_temp_out[i] == 1) 
    		p_count++;
    	if(out[i] == 1)
    		out_count++;
    	if ((pack_temp_out[i] != out[i]))
    	    cout << "error in bit " <<  i << endl;
    }
    cout<<"P_count is "<<p_count<<endl;
    cout<<"out count is "<<out_count<<endl;	//Printing out the count of set bits of out and pack_temp_out
    //comparing out and pack_temp_out
    for(int i=0;i<256;i++)
    {
        if(out[i] != pack_temp_out[i]) //If the bits don't match, break the loop. The function is not 100% correct  now since we have bit arrangement issues.
        {
            cout<<endl<<"The bit at position "<<i<<" doesn't match"<<endl;
            cout<<endl<<"Exiting the testing phase"<<endl;
            flag = 0;
            break;
        }
    }
    if(flag == 0)
    {
        cout<<endl<<"The outputs didn't matched, the function is not correct"<<endl; //prints if the break statement is not executed
    }
    else
        cout<<endl<<"The function is working fine"<<endl;
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
    generate_input(input,generator);

    //generate_debug_input(input);
   // generate_debug_key(key);

    mat_vec_mult(input, key, out);
    
    compute(key,input, output); // matrix-vector multiply mod 2

    //compute_debug(key,input, output); // matrix-vector multiply mod 2

    final_test(output,out);//compares the output of two different approaches
	return 0;
}
