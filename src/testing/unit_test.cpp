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
 * Generate two random matrices - one with the MSB's and one with the LSB's
 * since the resuoting matrix is 81X256, we need 2x256 words
 * The combination of the matrices are numbers in Z_3, so we need to ensure that we do not have the number 11
 * in any of the matching MSB's and LSB's
 *
 */
void generate_rand_matrix(uint64_t randMat1[2][256], uint64_t randMat2[2][256], std::mt19937 &generator)
{
	int temp_var;
    //for each word of the column, as we have 81 columns, so we need two 64-bit words for each
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 256; j++) {
            int nBitsGenerated = 0;
            while (nBitsGenerated < wLen)  //we need two words for each column in the two MSB and LSB matrices we are filling
            {
                uint64_t wGen = generator();
                //k is the index within the wGen, which holds a random 64 bit word
                for (int k = 0; k < wLen; k = k + 2) {

                    //if we already have 81 rows, the rest of the items in this column are set to 0
                    if ((i*wLen+k) >= 81) {
                        randMat1[i][j]=0;  //initialize to 0 - we do not need any more items
                        randMat2[i][j]=0;  //initialize to 0 - we do not need any more items
                        continue; //go to the next item
                    }

                    //examine each two bits and make sure they are not 11
                    int bit1 = (wGen >> k) & 1;
                    int bit2 = (wGen >> (k + 1)) & 1;

                    //make sure we don't have 11 - this is a mod 2 matrix so we can only have 00, 01 or 10
                    if (!((bit1 == 1) & (bit2 == 1)))
                    {
                    	//cout<<bit1<<"\t"<<bit2<<((bit1<<1) | bit2)<<endl;

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
            cout<<endl;

        }
    }
    //displaying what is inside randMat1 and randMat2
    int x,y;
    cout<<"Displaying the contents of randMat1 and randMat2 : after storing";
    for(int i = 0; i< 2; i++)
    {
    	for(int j = 0; j<256; j++)
    	{
    		for(int k =0;k<64;k++)
    		{
    			x = ((randMat1[i][j] >> k) & 1);
    			y = ((randMat2[i][j] >> k) & 1);
    			cout<<x<<"\t"<<y<<"\t"<<(( x<<1) | y)<<"\t";
    		}
    	}
    	cout<<endl;
    }
}
void mat_assemble(uint64_t msbs[2][256], uint64_t lsbs[2][256], int z3_mat[81][256])
{
	int z3_bit; //bits from lsb and msb are extracted and combined as a z3 bit.
    int bit_msb; //bit from each word of msb matrix i.e. randMat1
    int bit_lsb; //bit from each word lsb matrix i.e. randMat2
    uint64_t msb_word, lsb_word; //extracting each word of the 
    int current_row = 0;

    int cnt = 0;
    int row_limit = 81;

    //check the size of matrix msbs
    for(int col_count = 0; col_count < 256; col_count++)
    {
    	for(int row_count = 0; row_count < 2; row_count++)
    	{
    		msb_word = msbs[row_count][col_count];
            lsb_word = lsbs[row_count][col_count];
            for(int word_count = 0; word_count < wLen; word_count++)
            {
            	bit_msb = ((msb_word>>word_count) & 1);
            	bit_lsb = ((lsb_word>>word_count) & 1);
            	z3_bit = ((bit_msb << 1) | bit_lsb);
            	cout<<z3_bit;
            	current_row = wLen*row_count + word_count;
            	if(current_row < 81)
            	{
            		z3_mat[current_row][col_count] = z3_bit;
            	}
            	else
            		break;
            }
    	}
    }
    /*displaying the z3_mat
    for(int i =0; i< 4; i++)
    {
    	for(int j = 0; j< 4; j++)
    	{
    		cout<<z3_mat[i][j];
    	}
    	cout<<endl;
    }*/



    /*for(int i = 0; i<2;i++)
    {
        for(int j = 0; j < 256; j++)
        {
            msb_word = msbs[i][j];
            lsb_word = lsbs[i][j];
            for(int word_cnt = 0; word_cnt < wLen; word_cnt++)
            {
                bit_msb = (msb_word>>word_cnt) & 1;
                bit_lsb = (lsb_word>>word_cnt) & 1;
                //cout<<bit_msb<<"\t"<<bit_lsb<<endl;
                z3_bit = (bit_msb<<1 | bit_lsb);
                cout<<z3_bit<<endl;
                z3_mat[][]
            }
        }
        //z3_mat[j*][](msb_word>>k) & 1;
        //z3_mat[]
        //cout<<endl<<msb_word<<"\t"<<lsb_word<<endl;
    }
    for(int i = 0; i<256;i++)
    {
        for(int j = 0;j<2;j++)
        {
            msb_word = msbs[j][i];
            lsb_word = lsbs[j][i];
            cout<<endl<<msb_word<<"\t"<<lsb_word<<endl;
        }
    }*/
}

//word-packed version of phase 1

void compute(uint64_t key[4][256], uint64_t input[4], uint64_t z_final[4])
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

//naive version of phase 1
void mat_vec_mult(uint64_t input[4], uint64_t key[4][256], int out[256])
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
        /*if (iRow==32)
            cout << tempVar << "\n";*/
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

//Perform Addition modulo 3
void addMod3(uint64_t& outM, uint64_t& outL, uint64_t msb1, uint64_t lsb1, uint64_t msb2, uint64_t lsb2)
{
    uint64_t T = (lsb1 | msb2) ^ (lsb2 | msb1);

    outM = (lsb1 | lsb2 ) ^ T;
    outL = (msb1 | msb2 ) ^ T;
}
void unpackOutput(uint64_t output[4], char p2output[256])
{
    for (int j = 0; j < 4; j++) 
    {
        for (int i = 0; i < wLen; i++) 
        {
            p2output[i+j*wLen] = ((output[i] >> j) & 1);
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
    {
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

/*
* The function compares the output of unpacked version and output of naive implementation
*/
void final_test(uint64_t z_final[4], int out[256])
{
    bool flag = 0;// comparing out and pack_temp_out
    int pack_temp_out[256];
    int cnt=0;
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 64; j++)
        {
            pack_temp_out[64*i+j] = ((z_final[i] >> j) & 1); //4 packed int are converted to 256 z2 bits.
        }
    }
    int p_count = 0; //count of set bits in pack_temp_out
    int out_count = 0;  //count of set bits in naive output
    /*for(int i = 0; i < 256; i++)
    {
        cout<<pack_temp_out[i]<<"\t"<<out[i]<<endl;// printing the two outputs index by index
        Apart from comparing out and pack_temp_out, we also count the number of set bits
        if(pack_temp_out[i] == 1) 
            p_count++;
        if(out[i] == 1)
            out_count++;
        if ((pack_temp_out[i] != out[i]))
            cout << "error in bit " <<  i << endl;
    }
    cout<<"P_count is "<<p_count<<endl;
    cout<<"out count is "<<out_count<<endl; //Printing out the count of set bits of out and pack_temp_out*/
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
    else
        cout<<endl<<"Everything's good"<<endl;
}
    


//Step 1: Generate random input and store it in input array.
//Step 2: Generate 1024 uint64 numbers and store it in 4X256 size.
//Step 3: compute() function will calculate the value of zi's
int main()
{
    //===========General===============
    int choice; //choice for choosing the type of testing required
    //========Phase 1====================
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
    
    generate_input(input,generator);
    generate_rand_key(key, generator);
    generate_rand_matrix(randMat1, randMat2, generator);
    
    //mat_assemble(randMat1, randMat2, z3_mat);
    //mat_vec_mult(input, key, out);
    //compute(key,input, output); // matrix-vector multiply mod 2
    //final_test(output,out);//compares the output of two different approaches
    
    //unpackOutput(output,p2output); // useless operation that should not be here
    //multMod3(outM, outL, randMat1, randMat2, output); // matrix-vector multiply mod 3

    
    return 0;
}