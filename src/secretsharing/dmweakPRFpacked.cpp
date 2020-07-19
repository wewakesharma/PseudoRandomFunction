#include <iostream>
#include <random>
#include <bitset>
#include <cmath>
#include <chrono> 
#include <ctime> 
#include <cstdlib>

//#include "dmweakPRF.h"
#include "dmweakPRFpacked.h"

//unsigned long int z_final[4];//to store the final product values

using namespace std;

//
/*
 * Generate 4 words of random data into the input array.
 */
void generate_rand_input(uint64_t input[4], std::mt19937 &generator)
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
void generate_rand_Z3_matrix_81x256(uint64_t randMat1[2][256], uint64_t randMat2[2][256], uint64_t randMatZ3[81][256], std::mt19937 &generator)
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
}

void generate_test_Z3_matrix_81x256(uint64_t randMat1[2][256], uint64_t randMat2[2][256], uint64_t randMatZ3[81][256], std::mt19937 &generator)
{

    int jColTest = 1;
    int jRowsTest = 1;
    int nBitsGenMax = 1; //81 by default

    for (int jCol = jColTest; jCol < 256; jCol++) {
        //go over the columns
        for (int iRow = jRowsTest; iRow < 2; iRow++) {
            randMat1[iRow][jCol]=0;
            randMat2[iRow][jCol]=0;
        }
    }

    for (int jCol = 0; jCol < 256; jCol++)
        for (int iRow = 0; iRow < 81; iRow++)
        {
        randMatZ3[iRow][jCol] = 0; //default initialization
    }


    //for each word of the column, as we have 81 columns, so we need two 64-bit words for each
    for (int jCol = 0; jCol < jColTest; jCol++) {
        //go over the columns
        for (int iRow = 0; iRow < jRowsTest; iRow++) {
            //fill the numbers for each row

            randMat1[iRow][jCol]=0;
            randMat2[iRow][jCol]=0;

            int nBitsGenerated = 0;
            while (nBitsGenerated < wLen)  //we need two words for each column in the two MSB and LSB matrices we are filling
            {
                if ((iRow*wLen+nBitsGenerated) >= nBitsGenMax) {
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
}


/*
 * compute the multiplication of the key with the input
 * Each word holds 64 packed binary bits
 */
void wordPackedVecMatMult(uint64_t key[4][256], uint64_t input[4], uint64_t z_final[4])
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


void addMod3Alternative(uint64_t& outM, uint64_t& outL, uint64_t msb1, uint64_t lsb1, uint64_t msb2, uint64_t lsb2)
{

    outM = ((msb1 ^ msb2) & (~lsb1) & (~lsb2)) | ((~msb1) & (~msb2) & lsb1 & lsb2);
    outL = ((~msb1) & (~msb2) &  (lsb1 ^ lsb2)) | (msb1 & msb2 & (~lsb1) & (~lsb2));
}

void subMod3(uint64_t& outM, uint64_t& outL, uint64_t msb1, uint64_t lsb1, uint64_t msb2, uint64_t lsb2)
{
    addMod3(outM, outL, msb1, lsb1, lsb2, msb2); //this is equal to subtract, since
    //  (msb lsb) == -(lsb msb)    mod 3
}
void addMod3(uint64_t& outM, uint64_t& outL, uint64_t msb1, uint64_t lsb1, uint64_t msb2, uint64_t lsb2)
{
    uint64_t T = (lsb1 | msb2) ^ (lsb2 | msb1);

    outM = (lsb1 | lsb2 ) ^ T;
    outL = (msb1 | msb2 ) ^ T;
}

void addMod3vec4(uint64_t msb1[4], uint64_t lsb1[4], uint64_t msb2[4], uint64_t lsb2[4], uint64_t outM[4], uint64_t outL[4])
{

    for (int i = 0; i < 4; i++) {
        uint64_t T = (lsb1[i] | msb2[i]) ^(lsb2[i] | msb1[i]);

        outM[i] = (lsb1[i] | lsb2[i]) ^ T;
        outL[i] = (msb1[i] | msb2[i]) ^ T;
    }

}

void addMod3vec2(uint64_t msb1[2], uint64_t lsb1[2], uint64_t msb2[2], uint64_t lsb2[2], uint64_t outM[2], uint64_t outL[2])
{

    for (int i = 0; i < 4; i++) {
        uint64_t T = (lsb1[i] | msb2[i]) ^(lsb2[i] | msb1[i]);

        outM[i] = (lsb1[i] | lsb2[i]) ^ T;
        outL[i] = (msb1[i] | msb2[i]) ^ T;
    }

}

void unpackOutput(uint64_t output[4], char p2output[256])
{
    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < wLen; i++) {
            p2output[i+j*wLen] = ((output[i] >> j) & 1) ;
        }
    }
}

void unpackOutputMod3(uint64_t output[12], uint64_t p2output[84])
{
    for (int i = 0; i < 12; i++) {
        uint64_t tmp = output[i];
        for (int jWord = 6; jWord >=0; jWord--) {

            uint64_t outputNum= (tmp & 0x1ff) % 3;
            p2output[i*7+jWord] = outputNum;  //result needs to be mod 3
            tmp = (tmp>> 9);
        }
    }
}

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
void packWords(uint64_t randPackedWords[12][256] ,uint64_t randMatZ3[128][256]){

 //   cout << "expecting 12x256, 128x256 words" << endl;
    uint64_t acc = 0;

    for (int jCol = 0; jCol < 256; jCol++) {
        int inWordStart = 0; //index into the beginning of the next word in the column
        for (int joutWordIndex = 0; joutWordIndex < 12; joutWordIndex++) { //we will have 12 output words
            acc = 0;
            for (int i = 0; i < 7; i++) {
                if (inWordStart + i == 81) {
                    acc = acc << 27;//(9*((84 - inWordStart+i))); //our vector is 84 bit long, so we need to adjust for this
                    break;
                }
                acc = (acc << 9) + randMatZ3[inWordStart + i][jCol];
            }
            inWordStart += 7;
            randPackedWords[joutWordIndex][jCol] = acc;
        }
    }
}

/*
 * randPackedWords has words packed in columns - each word contains 7 values, each value is up to 512
 *
 */
void extractMatrixOutputWords(uint64_t uOutPacked[12][256], uint64_t uInUnpacked[81][256])
{
    for (int iCol = 0; iCol < 256; iCol++) {
        for (int iRow = 0; iRow < 12; iRow++) {
            uint64_t in = uInUnpacked[iRow][iCol];
            for (int iIndinCol = 7*iRow+6; iIndinCol >= 7*iRow; iIndinCol--) {
                if (iIndinCol >=81)
                    break;
                uOutPacked[iIndinCol][iCol] = (in & 0X1FF) % 3;
                in >>=9;
            }
        }
    }
}


void MultPackedMatIn2(uint64_t inMat[12][256],uint64_t inVec[4], uint64_t outVec[12])
{
    for (int i = 0; i < 12; i++) {
        outVec[i] = 0;
    }

    for (int j1 = 0; j1 < 4; j1++)
    {
        //uint64_t tmp = inVec[j1];  //take each word in the input vector
        for (int j2 = 0; j2 < wLen; j2++)
        {
            uint64_t bit = ((inVec[j1] >> j2) & 1);

            for (int i = 0; i < 12; i++) {
                //this seems to be slower than the first alternative
                uint64_t product = (-1 * bit) & inMat[i][(j1*wLen)+j2];
                //uint64_t product += bit & inMat[i][j1*wLen+j2];
                outVec[i]+=product;
            }
        }
    }
}


/*
 *
 *  cout << "expecting  12X256, 4, 12 words" << endl;
 */
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

/*
 *
 *
 */

void InnerProdMul2(uint64_t outVec[84], uint64_t randMatZ3[128][256], uint64_t in[4]) {

    uint64_t randPackedWords[12][256];
    uint64_t outVecPacked[12];

    //  cout << "sending 12x256, 128x256 words" << endl;
    packWords(randPackedWords,randMatZ3);

    // uint64_t randUnpackedWords[84][256];
    //  extractMatrixOutputWords(randPackedWords, randUnpackedWords); //for debugging purposes

    //multiply the words here
    //   cout << "sending  12X256, 4, 12 words" << endl;
    MultPackedMatIn2(randPackedWords,in,  outVecPacked);

    unpackOutputMod3(outVecPacked, outVec);

    //compare taht the extracted words are equal to the previous matrix

}

void InnerProdMul(uint64_t outVec[84], uint64_t randMatZ3[128][256], uint64_t in[4]) {

    uint64_t randPackedWords[12][256];
    uint64_t outVecPacked[12];

  //  cout << "sending 12x256, 128x256 words" << endl;
    packWords(randPackedWords,randMatZ3);

   // uint64_t randUnpackedWords[84][256];
  //  extractMatrixOutputWords(randPackedWords, randUnpackedWords); //for debugging purposes

    //multiply the words here
 //   cout << "sending  12X256, 4, 12 words" << endl;
    MultPackedMatIn(randPackedWords,in,  outVecPacked);

    unpackOutputMod3(outVecPacked, outVec);

    //compare taht the extracted words are equal to the previous matrix

}


/*
 * multMod3
 * Multiply a trinery matrix with a binary vector
 */

void multMod3_Z3Mat_Z2Vec(uint64_t msbs[2][256], uint64_t lsbs[2][256], uint64_t in[4], uint64_t outM[2], uint64_t outL[2])
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

void multMod3_Z3Mat_Z3Vec2(uint64_t msbs[2][256], uint64_t lsbs[2][256], uint64_t inm[4], uint64_t inl[4], uint64_t outM[2], uint64_t outL[2]) {
    uint64_t msbbitm[2], lsbbitm[2];
    uint64_t msbbitl[2], lsbbitl[2];

    uint64_t outMbitm[2], outLbitm[2];
    uint64_t outMbitl[2], outLbitl[2];
    uint64_t acc1[2];
    uint64_t acc2[2] ;
    //go over the input bits, one by one

    for (int j = 0; j < 2; j++)
    {
        acc1[j] = 0;
        acc2[j] = 0;
    }

    for (int i1 = 0; i1 < 4; i1++)
        for (int i2 = 0; i2 < 64; i2++) {
            uint64_t bitm = -((inm[i1] >> i2) & 1); //the input bit, replicated either all=0 or all=1
            uint64_t bitl = -((inl[i1] >> i2) & 1); //the input bit, replicated either all=0 or all=1



            uint64_t tmpm[2], tmpl[2];

            for (int j = 0; j < 2; j++) //each column has 81 rows, so need 2 64-bit words
            {
                tmpm[j] = msbs[j][64 * i1 + i2];
                tmpl[j] = lsbs[j][64 * i1 + i2];


                addMod3(outMbitm[j], outMbitl[j], outMbitm[j], outMbitl[j], msbbitm[j],
                        lsbbitm[j]); //add mod 3 to acumulator

                msbbitl[j] = msbs[j][64 * i1 + i2] & bitl;
                lsbbitl[j] = lsbs[j][64 * i1 + i2] & bitl;  //multiply by current bit

                uint64_t m1 = (tmpm[j] & bitm) & (tmpl[j] & bitl);
                uint64_t m2 = (tmpl[j] & bitm) & (tmpm[j] & bitl);

                addMod3(acc1[j], acc2[j], acc1[j], acc2[j], m1, m2);


            }
        }

    for (int i = 0; i < 2; i++) {
    outM[i] = acc1[i];
    outL[i] = acc2[i];
}
}

void multMod3_Z3Mat_Z3Vec(uint64_t msbs[2][256], uint64_t lsbs[2][256], uint64_t inm[4], uint64_t inl[4], uint64_t outM[2], uint64_t outL[2])
{
    uint64_t msbbitm[2], lsbbitm[2];
    uint64_t msbbitl[2], lsbbitl[2];

    uint64_t outMbitm[2], outLbitm[2];
    uint64_t outMbitl[2], outLbitl[2];

    //go over the input bits, one by one

    for (int i1 = 0; i1 < 4; i1++)
        for (int i2 = 0; i2 < 64; i2++)
        {
            uint64_t bitm = -((inm[i1] >> i2 ) & 1 ); //the input bit, replicated either all=0 or all=1
            uint64_t bitl = -((inl[i1] >> i2 ) & 1 ); //the input bit, replicated either all=0 or all=1

            for (int j = 0; j < 2; j++) //each column has 81 rows, so need 2 64-bit words
            {
                msbbitm[j] = msbs[j][64*i1+i2] & bitm;
                lsbbitm[j] = lsbs[j][64*i1+i2] & bitm;  //multiply by current bit
                addMod3(outMbitm[j],outMbitl[j],outMbitm[j],outMbitl[j],msbbitm[j],lsbbitm[j]); //add mod 3 to acumulator

                msbbitl[j] = msbs[j][64*i1+i2] & bitl;
                lsbbitl[j] = lsbs[j][64*i1+i2] & bitl;  //multiply by current bit
                addMod3(outLbitm[j],outLbitl[j],outLbitm[j],outLbitl[j],msbbitl[j],lsbbitl[j]); //add mod 3 to acumulator

            }
        }

    //subtract the msb output from the lsb - mod 3 calculation:
    //R = 2*a+b (mod 3)= b - a (mod 3)

    uint64_t outMbitmneg[2], outMbitlneg[2];

    for (int i = 0; i < 2; i++)
    {
        outMbitmneg[i] = ~outMbitm[i];
        outMbitlneg[i] = ~outMbitl[i];
    }

    addMod3vec2(outM,outL,outLbitm,outLbitl,outMbitmneg,outMbitlneg);
}






