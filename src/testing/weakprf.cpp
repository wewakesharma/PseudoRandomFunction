//cpp file for the header file

#include <iostream>
#include <random>
#include <bitset>
#include <cmath>
#include <chrono> 
#include <ctime> 
#include <cstdlib>
#include "weakprf.h"

int wLen = 64;


//unsigned long int z_final[4];//to store the final product values

using namespace std;

/*
 * Generate 4 words of random data into the input array.
 */
void generate_input(uint64_t (&input)[4], std::mt19937 &generator)
{
    //srand(time(NULL));
    for(int i = 0; i < 4; i++)
    {
        //Call the generator that generate a 64-bit word each time
        input[i] = generator();
    }
    /*DEBUGGING CODE
    cout<<endl<<"Printing out the values of input"<<endl;DEBUG
    for(int i =0; i<4;i++)
    {
        cout<<input[i]<<endl;
    }*/
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

void generate_rand_matrix(uint64_t (&randMat1)[2][256], uint64_t (&randMat2)[2][256], uint64_t (&randMatZ3)[81][256], std::mt19937 &generator)
{
    //for each word of the column, as we have 81 columns, so we need two 64-bit words for each
    for (int jCol = 0; jCol < 256; jCol++) {
        //go over the columns
        for (int iRow = 0; iRow < 2; iRow++) {
            //fill the numbers for each row

            randMat1[iRow][jCol]=0;
            randMat2[iRow][jCol]=0;

            int nBitsGenerated = 0;
            while (nBitsGenerated < wLen)  //we need two words for each column in the two MSB and LSB matrices we are filling
            {
                if ((iRow*wLen+nBitsGenerated) >= 81) {
                    break; //go to the next item
                }

                uint64_t wGen = generator();
                //k is the index within the wGen, which holds a random 64 bit word
                for (int k = 0; k < wLen; k = k + 2) {

                    //if we already have 81 rows, the rest of the items in this column are set to 0

                    //examine each two bits and make sure they are not 11
                    uint64_t bit1 = (wGen >> k) & 1;
                    uint64_t bit2 = (wGen >> (k + 1)) & 1;

                    //make sure we don't have 11 - this is a mod 2 matrix so we can only have 00, 01 or 10
                    if (!((bit1 == 1) & (bit2 == 1)))
                    {
                        //assign the next bits generated to their locations in the random matrices, the location is nBitsFound
                        randMat1[iRow][jCol] |= (bit1 << nBitsGenerated);
                        randMat2[iRow][jCol] |= (bit2 << nBitsGenerated);
                        randMatZ3[nBitsGenerated+iRow*wLen][jCol]=(bit1<<1 | bit2);
                        nBitsGenerated++;
                        //if we reached the end of the random matrix word, we need to go to the next word. We will then generate a new random word to fill it
                        if ((nBitsGenerated == wLen) || (iRow*wLen+nBitsGenerated) == 81) {
                            break; //go to the next item
                        }
                    }
                }
            }
        }
    }
    /*DEBUGGING CODE
    cout<<endl<<"Printing out the values of randMatZ3"<<endl;
    for(int i = 0;i<81;i++)
    {
        for(int j = 0; j<256;j++)
        {
            cout<<randMatZ3[i][j]<<" ";
        }
        cout<<endl;
    }*/
}


//Performs the computation of phase 1 by wordpacking. This method is described in /Documents/Wordpacking-Example(new).pdf

void compute(uint64_t (&key)[4][256], uint64_t (&input)[4], uint64_t (&z_final)[4])
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
                uint64_t z = key[k][j+(wLen*i)];
                z_final[k] ^= (z & y);
            }
        }
    }
}



//TESTING - Performs naive vector matrix multiplication of phase 1 

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


// TESTING - The function compares the output of unpacked version and output of naive implementation- phase 1 testing

void phase1_test(uint64_t (&z_final)[4], uint64_t (&naive_out_p1)[256])
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
        if(naive_out_p1[i] != pack_temp_out[i]) //If the bits don't match, break the loop. The function is not 100% correct  now since we have bit arrangement issues.
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
    /* DEBUGGING
    cout<<endl<<"Printing the output of phase 1"<<endl; 
    for(int i = 0; i< 256; i++)
    {
        cout<<pack_temp_out[i]<<"\t"<<naive_out_p1[i]<<endl;
    }*/
}


//Perform Addition modulo 3
void addMod3(uint64_t& outM, uint64_t& outL, uint64_t msb1, uint64_t lsb1, uint64_t msb2, uint64_t lsb2)
{
    uint64_t T = (lsb1 | msb2) ^ (lsb2 | msb1);

    outM = (lsb1 | lsb2 ) ^ T;
    outL = (msb1 | msb2 ) ^ T;
}


// Computes phase 3 using wordpacking as shown in the repo, in /Documents/Wordpacking-Example2.pdf. The output is stored in p3_out(81 bits in Z3)
 
void multMod3(uint64_t (&outM)[2], uint64_t (&outL)[2], uint64_t (&msbs)[2][256], uint64_t (&lsbs)[2][256], uint64_t (&in)[4], uint64_t (&p3_out)[81])
{
    uint64_t bit_msb, bit_lsb, z3_bit;
    uint64_t msb[2], lsb[2];
    int row_limit = 81;

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
    //The 81 z3 bits from wordpacked method
    for(int row_count = 0; row_count < 2; row_count++)
    {
        for(int word_count = 0; word_count < wLen; word_count++)
        {
            if(row_count*wLen+word_count < row_limit)
            {
                bit_msb = ((outM[row_count]>>word_count) & 1); //extracting bit from OUTM
                bit_lsb = ((outL[row_count]>>word_count) & 1);//extracting bit from OUTL
                z3_bit = ((bit_msb<<1) | bit_lsb); //combining the bits to form a z3 element
                p3_out[row_count*wLen+word_count] = z3_bit;
            }
        }
    }  
}

//compute the product of 81x256 z3 matrix with 256 bit output vector of phase 1

void phase3_naive(uint64_t (&naive_out_p1)[256], uint64_t (&randMatZ3)[81][256], uint64_t (&naive_out_p3)[81])//naive version of phase 3
{
    int prod;
    for(int row_count = 0; row_count < 81; row_count++)
    {
        int sum_of_product = 0;
        for(int col_count = 0; col_count < 256; col_count++)
        {
            sum_of_product += (naive_out_p1[col_count] * randMatZ3[row_count][col_count]);
        }
        naive_out_p3[row_count] = (sum_of_product % 3);
    }
}


//Compare the output of multMod3 and phase3_naive and tests the correctness of phase3 computation

void phase3_test(uint64_t (&naive_out_p3)[81], uint64_t (&p3_out)[81])//compares p3_out(wordpacked) and phase3_out(naive)
{
    bool phase3_flag = 0;// 0 stands from successful matching
    int row_limit = 81; //number of rows required in final output
    for(int row_count = 0; row_count < row_limit; row_count++)
    {
        if (naive_out_p3[row_count]!=p3_out[row_count])
        {
            cout << "Something is wrong in index " << row_count << endl;
            cout<<endl<<"!!!-Exiting the testing phase-!!!"<<endl;
            phase3_flag = 1;
            break;
        }  
    }
    if(phase3_flag == 1)
        cout<<"The computational outputs using naive and wordpacking DID NOT match, the function is NOT correct ==========> ERROR"<<endl; //prints if the break statement is not executed
    else
        cout<<"Output of naive and word-packed version in phase 3 MATCHED ==========> O.K."<<endl;
    cout<<endl<<"Printing both output of phase3"<<endl;
    /* DEBUGGING CODE
    for(int i = 0;i<81;i++)
    {
        cout<<naive_out_p3[i]<<"\t"<<p3_out[i]<<endl;
    }*/
}

/*
*PRF using Alternate Methods using Integer packing
*/
/*
 * Pack the original Z_3 values into words where each word includes 7 original values
 * we originally multiply a Z_3 matrix of width 256 with possible values 0,1,2 with a
 * vinary vector of size 256 with possible values of 0,1
 * Therefore, the maximum value of the computation can be 512
 * Therefore, each value will be packed into 9 bits total, where the 7 of them will have the value 0
 */
/*   cout << "sending 12x256, 128x256 words" << endl;
 *
 */
void packWords(uint64_t randPackedWords[12][256] ,uint64_t randMatZ3[81][256]){

 //   cout << "expecting 12x256, 128x256 words" << endl;
    int acc = 0;
    int inWordStart = 0; //index into the beginning of the next word in the column

    for (int jCol = 0; jCol < 256; jCol++) {
        for (int joutWordIndex = 0; joutWordIndex < 12; joutWordIndex++) { //we will have 12 output words
            acc = 0;
            for (int i = 0; i < 7; i++) {
                if (inWordStart + i >= 81)
                    break;
                acc = (acc << 9) + randMatZ3[inWordStart + i][jCol];
            }
            inWordStart += 7;
            randPackedWords[joutWordIndex][jCol] = acc;
        }
    }
}

void MultPackedMatIn(uint64_t inMat[12][256],uint64_t inVec[4], uint64_t outVec[12])
{

    for (int i = 0; i < 12; i++)
    {
        outVec[i] = 0;
        for (int j1 = 0; j1 < 4; j1++)
        {
            uint64_t tmp = inVec[j1];  //take each word in the input vector
            for (int j2 = 0; j2 < wLen; j2++)
            {
                uint64_t bit = tmp & 1;

                tmp >>= 1;    //take the next bit in this word
                //alternative way, but may be slower
                uint64_t product = bit * inMat[i][(j1*wLen)+j2];
                //this seems to be slower than the first alternative
                //uint64_t product = (-1 * bit) & inMat[i][(j1*wLen)+j2];
                outVec[i] += product;
            }
        }
    }
}
void InnerProdMul(uint64_t (&outVec)[12], uint64_t (&randMatZ3)[81][256], uint64_t (&in)[4])
{
    uint64_t randPackedWords[12][256];

  //  cout << "sending 12x256, 128x256 words" << endl;
    packWords(randPackedWords,randMatZ3);

   // uint64_t randUnpackedWords[84][256];
  //  extractMatrixOutputWords(randPackedWords, randUnpackedWords); //for debugging purposes

    //multiply the words here
 //   cout << "sending  12X256, 4, 12 words" << endl;
    MultPackedMatIn(randPackedWords,in,  outVec);

    //compare taht the extracted words are equal to the previous matrix
    cout<<endl<<"Printing the value for outVec"<<endl;
    for(int i = 0;i<12;i++)
    {
        cout<<outVec[i]<<endl;
    }
}



