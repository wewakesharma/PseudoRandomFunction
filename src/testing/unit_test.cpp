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
	uint64_t temp_mat1, temp_mat2, temp_var;
	int cnt = 0;
    //for each word of the column, as we have 81 columns, so we need two 64-bit words for each
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 256; j++) {
            int nBitsGenerated = 0;
            temp_mat1 = 0;
            temp_mat2 = 0;
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
}

/*
*Stands for matrix assembly, this will take two matrices and combine their bits to form a single valued z3 element
*this element will be stored in z3_mat which will be used in testing phase 3.
*/
void mat_assemble(uint64_t msbs[2][256], uint64_t lsbs[2][256], uint64_t z3_mat[81][256])
{
	uint64_t msb_word, lsb_word; //extracting each word
   	uint64_t msb_bit; //bit from each word of msb matrix i.e. randMat1
    uint64_t lsb_bit; //bit from each word lsb matrix i.e. randMat2
    uint64_t z3_bit; //bits from lsb and msb are extracted and combined as a z3 bit.
    
    int current_row = 0;
    int cnt = 0;
    int row_limit = 81;

    for(int col_count = 0; col_count < 256; col_count++)
    {
    	current_row = 0;
    	for(int row_count = 0; row_count < 2; row_count++)
    	{
    		msb_word = msbs[row_count][col_count];
            lsb_word = lsbs[row_count][col_count];
            for(int word_count = 0; word_count < wLen; word_count++)
            {
            	msb_bit = ((msb_word>>word_count) & 1);
            	lsb_bit = ((lsb_word>>word_count) & 1);
            	z3_bit = ((msb_bit << 1) | (lsb_bit <<0)) ;
            	//cout<<z3_bit;
            	current_row = wLen*row_count + word_count;
            	if(current_row < 81)
            		z3_mat[current_row][col_count] = z3_bit;
            	else
            		break;
            }
    	}
    }
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

//Naive version of phase 1
void mat_vec_mult(uint64_t input[4], uint64_t key[4][256], uint64_t out[256])
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
    //printing the output for naive testing of phase 1
    /*cout<<endl<<"Output of phase 1 using naive method"<<endl;
    for(int count = 0; count < 256; count++)
    {
    	cout<<out[count];
    }*/
    cout<<endl;
}

//Perform Addition modulo 3
void addMod3(uint64_t& outM, uint64_t& outL, uint64_t msb1, uint64_t lsb1, uint64_t msb2, uint64_t lsb2)
{
    uint64_t T = (lsb1 | msb2) ^ (lsb2 | msb1);

    outM = (lsb1 | lsb2 ) ^ T;
    outL = (msb1 | msb2 ) ^ T;
    cout<<endl<<"Output of phase 3"<<endl;
    cout<<outM<<endl;
    cout<<outL<<endl;
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
    //printing the output of phase 1 using word packing method
}

/*
 * multiply modulo 3
 */
void multMod3(uint64_t outM[2], uint64_t outL[2], uint64_t msbs[2][256], uint64_t lsbs[2][256], uint64_t in[4])
{
    uint64_t msb[2], lsb[2];
    int iter_count = 0;
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
                iter_count++;
            }

        }
    }
    cout<<endl<<"Iter count is "<<iter_count<<endl;
        
}

/*
* The function compares the output of unpacked version and output of naive implementation- phase 1 testing
*/
void final_test(uint64_t z_final[4], uint64_t out[256])
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
    //printing the output of wordpacked version
    /*cout<<endl<<"Output of phase 1 using word packing"<<endl;
    for(int count = 0; count < 256; count++)
    {
    	cout<<pack_temp_out[count];
    }*/

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
        cout<<cout<<endl<<"The outputs out and pack_temp_out DID NOT match, the function is not correct ==========> ERROR"<<endl; //prints if the break statement is not executed
    else
        cout<<endl<<"Output of naive and word-packed version MATCHED ==========> O.K."<<endl;
}
    
//compute the product of 81x256 z3 matrix with 256 bit output vector of phase 1
void phase3_naive(uint64_t out[256], uint64_t z3_mat[81][256], uint64_t phase3_out[81])
{
	int prod;
	int sum_of_product = 0;
	for(int row_count = 0; row_count < 81; row_count++)
	{
		for(int col_count = 0; col_count < 256; col_count++)
		{
			sum_of_product += (out[col_count] * z3_mat[row_count][col_count]);
		}
		phase3_out[row_count] = (sum_of_product % 3);
	}
	//displaying the output for phase3_out
	for(int count = 0; count < 81; count++)
	{
		cout<<phase3_out[count];
	}
	cout<<endl;
}

void phase3_test()
{
	cout<<"In Progress";
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
    uint64_t out[256];  //output of phase 1(naive)
    char p2output[256];
    //========== Phase 2/3==================
    uint64_t randMat1[2][256], randMat2[2][256];
    uint64_t outM[2];
    uint64_t outL[2];
    uint64_t z3_mat[81][256];
    uint64_t phase3_out[81];

    unsigned seed = 7;   

    std::mt19937 generator(seed); // mt19937 is a standard mersenne_twister_engine
    
    //Input Generation
    generate_input(input,generator);
    generate_rand_key(key, generator);
    generate_rand_matrix(randMat1, randMat2, generator);
    cout<<endl<<"Random input generated ==========>   O.K."<<endl;
    
    //Phase 1
    cout<<"Initializing unit testing for phase 1 ==========>   O.K."<<endl;
    compute(key,input, output); // matrix-vector multiply mod 2
    //unpackOutput(output,p2output); // useless operation that should not be here
    mat_vec_mult(input, key, out);
    final_test(output,out);//compares the output of two different approaches
    cout<<"==========Phase 1 testing complete=========="<<endl;

    //Phase 3
    cout<<"Initializing unit testing for phase 3 ==========>   O.K."<<endl;
    mat_assemble(randMat1, randMat2, z3_mat);
    //multMod3(outM, outL, randMat1, randMat2, output); // matrix-vector multiply mod 3
    phase3_naive(out, z3_mat, phase3_out);

    
    return 0;
}