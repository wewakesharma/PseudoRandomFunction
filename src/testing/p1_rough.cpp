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
 * Generate 4 words of random data and store in an uint64_t input array.
 */
void generate_input(uint64_t input[4], std::mt19937 &generator)
{
    for(int i = 0; i < 4; i++)
    {
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

//Testing code for phase 1 matrix vector multiplication using naive implementation.
void mat_vec_mult(uint64_t input[4], uint64_t key[4][256], bool out[256])
{
    int column_total = 0;
	for(int col_count = 0; col_count < 256; col_count++) 		//we have to traverse 256 columns in key matrix
    {
        column_total = 0;
        for(int row_count = 0; row_count < 4; row_count++) 			//traverses through 4 rows of each column in both key and input
        {
            for(int word_count = 0; word_count < wLen; word_count++)		//each bit of a word
            {
                column_total = column_total + (((input[row_count]>>word_count)&1) * ((key[row_count][col_count]>>word_count)&1));
            }
        }
        out[col_count] = (column_total%2);
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
                uint64_t z = key[k][j+wLen*i];//Take each key from the key matrix. row by row
                z_final[k] ^= (z & y);//bitwise AND with the y.
            }
		}
	}
}

void naive2_func(uint64_t key[4][256], uint64_t input[4], bool out2[256])
{
    //cout<<endl<<"We are in naive2 function"<<endl;
    int temp_out = 0;
    bool input_bit = 0;
    for(int j = 0; j < 256; j++)
    {
        temp_out = 0;
        for(int i=0; i < 4; i++)
        {
            for(int k = 0; k < 64; k++)
            {
                input_bit = (input[i]>>k) & 1;
                if(input_bit == 1 )
                {
                    temp_out += ((key[i][j]>>k)&1);
                } 
            }
        }
        //cout<<temp_out<<endl;
        out2[j] = (temp_out%2);
    }   
    //total_bit_count = bitCount(z_final);
    //return (total_bit_count%3);
}

/*
* The function compares the output of unpacked version and output of naive implementation
*/
void final_test(uint64_t z_final[4], bool out[256], bool out2[256])
{
    bool flag = 0;// out and temp_out
	bool pack_temp_out[256];
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

    	//cout<<pack_temp_out[i]<<"\t"<<out[i]<<endl;// printing the two outputs index by index
    	//**Apart from comparing out and pack_temp_out, we also count the number of set bits
    	if(pack_temp_out[i] == 1) 
    		p_count++;
    	if(out[i] == 1)
    		out_count++;
    }
    //cout<<"P_count is "<<p_count<<endl;
    //cout<<"out count is "<<out_count<<endl;	//Printing out the count of set bits of out and pack_temp_out
    //comparing out and pack_temp_out
    for(int i=0;i<256;i++)
    {
        if(out[i] != pack_temp_out[i]) //If the bits don't match, break the loop. The function is not 100% correct  now since we have bit arrangement issues.
        {
            cout<<endl<<"The bit at position "<<i<<" doesn't match"<<endl;
            cout<<endl<<"Exiting the testing phase"<<endl;
            flag = 1;
            break;
        }
    }
    if(flag == 1)
        cout<<cout<<endl<<"The outputs out and pack_temp_out didn't match, the function is not correct"<<endl; //prints if the break statement is not executed

    /*comparing out and out2
    for(int i=0;i<256;i++)
    {
        if(out[i] != out2[i]) //If the bits don't match, break the loop. The function is not 100% correct  now since we have bit arrangement issues.
        {
            cout<<endl<<"The bit at position "<<i<<" doesn't match"<<endl;
            cout<<endl<<"Exiting the testing phase"<<endl;
            flag2 = 1;
            break;
        }
        
    }
    
    if(flag2 == 1)
        cout<<endl<<"The outputs out and out2 didn't match, the function is not correct"<<endl; //prints if the break statement is not executed
        */
}
//=========
void naive2_test(bool out[256], bool out2[256])
{
    bool flag2 = 0;//out and out2
    //comparing out and out2
    for(int i=0;i<256;i++)
    {
        if(out[i] != out2[i]) //If the bits don't match, break the loop. The function is not 100% correct  now since we have bit arrangement issues.
        {
            cout<<endl<<"The bit at position "<<i<<" doesn't match"<<endl;
            cout<<endl<<"Exiting the testing phase"<<endl;
            flag2 = 1;
            break;
        }
        
    }
    
    if(flag2 == 1)
        cout<<endl<<"The outputs out and out2 didn't match, the function is not correct"<<endl; 
    else
        cout<<endl<<"The outputs out and out2 match, the function is correct"<<endl;//prints if the break statement is not executed
}
//========


//Step 1: Generate random input and store it in input array.
//Step 2: Generate 1024 uint64 numbers and store it in 4X256 size.
//Step 3: compute() function will calculate the value of zi's
int main()
{
	uint64_t key[4][256];
    uint64_t input[4];
    int unpacked_input[256];
    bool out[256];  //variable used for naive testing, represents output of phase 1
    bool out2[256];
    uint64_t output[4];
    //uint64_t temp_out[4];

    unsigned seed = 7;    // std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed); // mt19937 is a standard mersenne_twister_engine
	generate_rand_key(key, generator);

	//we generate two random matrices, one holds the first bit and one the second bit
    generate_input(input,generator);
    mat_vec_mult(input, key, out);
    
    compute(key,input, output); // matrix-vector multiply mod 2
    naive2_func(key,input,out2);//parallel naive implementation of the multiplication
    final_test(output,out, out2);//compares the output of two different approaches
    naive2_test(out, out2);
	return 0;
}
