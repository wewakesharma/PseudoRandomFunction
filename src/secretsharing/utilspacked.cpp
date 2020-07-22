#include <iostream>
#include <random>
#include <bitset>
#include <cmath>
#include <chrono> 
#include <ctime> 
#include <cstdlib>
#include <assert.h>


//unsigned long int z_final[4];//to store the final product values

using namespace std;

int wLen = 64;

void generate_test_packed_word(uint64_t tword, std::mt19937 &generator)
{

    tword = 0;

    //number of bits that are non-zero
    int jBitTest = 1;

    for (int j = 0; j < jBitTest; j++)
        //Call the generator that generate a 64-but word each time
        tword |= (generator()& 1) << j;

}


void generate_test_packed_vector_4(uint64_t vec[4], std::mt19937 &generator)
{

    for(int i = 0; i < 4; i++)
    {
        vec[i] = 0;
    }

    //number of bits that are non-zero
    int jBitTest = 1;
    int iWordTest = 1; //number of words that are non-zero
    //srand(time(NULL));
    for(int i = 0; i < iWordTest; i++)
    {
        for (int j = 0; j < jBitTest; j++)
        //Call the generator that generate a 64-but word each time
        vec[i] |= (generator()& 1) << j;
    }
}

void generate_rand_Z3_packed_Word(uint64_t& PackedWordm, uint64_t& PackedWordl, uint64_t (&unpackedWord)[64], std::mt19937 &generator)
{

    PackedWordm=0;
    PackedWordl=0;
    for(int i = 0;i<wLen;i++)
    {
        unpackedWord[i] = 0;
    }
    //for each word of the column, as we have 81 columns, so we need two 64-bit words for each
    for (int jCol = 0; jCol < 64; jCol++) {
        //go over the columns
        //fill the numbers for each row
        //unpackedWord[jCol] = 0;

        int nBitsGenerated = 0;
        while (nBitsGenerated < wLen)  //we need two words for each column in the two MSB and LSB matrices we are filling
        {

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
                    PackedWordm |= (bit1 << nBitsGenerated);
                    PackedWordl |= (bit2 << nBitsGenerated);
                    unpackedWord[nBitsGenerated]=(bit1<<1 | bit2);
                    nBitsGenerated++;
                    //if we reached the end of the random matrix word, we need to go to the next word. We will then generate a new random word to fill it
                    if (nBitsGenerated == wLen) {
                        break; //go to the next item
                    }
                }
            }
        }

    }

}

void generate_test_Z3_packed_Word(uint64_t& PackedWordm, uint64_t& PackedWordl, uint64_t (&unpackedWord)[64], std::mt19937 &generator)
{
    int jTestCols=1;
    PackedWordm=0;
    PackedWordl=0;
    for(int i = 0;i<64;i++)
    {
        unpackedWord[i] = 0;
    }
    //for each word of the column, as we have 81 columns, so we need two 64-bit words for each
    for (int jCol = 0; jCol < jTestCols; jCol++) {
        //go over the columns
        //fill the numbers for each row
        //unpackedWord[jCol] = 0;

        int nBitsGenerated = 0;
        while (nBitsGenerated < wLen)  //we need two words for each column in the two MSB and LSB matrices we are filling
        {

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
                    PackedWordm |= (bit1 << nBitsGenerated);
                    PackedWordl |= (bit2 << nBitsGenerated);
                    unpackedWord[nBitsGenerated]=(bit1<<1 | bit2);
                    nBitsGenerated++;
                    //if we reached the end of the random matrix word, we need to go to the next word. We will then generate a new random word to fill it
                    if (nBitsGenerated == wLen) {
                        break; //go to the next item
                    }
                }
            }
        }

    }

}

void generate_rand_packed_vector_4(uint64_t vec[4], std::mt19937 &generator)
{

    //srand(time(NULL));
    for(int i = 0; i < 4; i++)
    {
        //Call the generator that generate a 64-but word each time
        vec[i] = generator();
    }
}


void generate_test_Z3_packed_Vec_4(uint64_t PackedVecm[4], uint64_t PackedVecl[4], uint64_t unpackedVec[256], std::mt19937 &generator)
{
    int jTestCols=1;
    assert(jTestCols<4);

    //for each word of the column, as we have 81 columns, so we need two 64-bit words for each
    for (int jCol = 0; jCol < jTestCols; jCol++) {
        //go over the columns
        //fill the numbers for each row

        PackedVecm[jCol]=0;
        PackedVecl[jCol]=0;
        unpackedVec[jCol] = 0;

        int nBitsGenerated = 0;
        while (nBitsGenerated < wLen)  //we need two words for each column in the two MSB and LSB matrices we are filling
        {

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
                    PackedVecm[jCol] |= (bit1 << nBitsGenerated);
                    PackedVecl[jCol] |= (bit2 << nBitsGenerated);
                    unpackedVec[jCol*wLen+nBitsGenerated]=(bit1<<1 | bit2);
                    nBitsGenerated++;
                    //if we reached the end of the random matrix word, we need to go to the next word. We will then generate a new random word to fill it
                    if (nBitsGenerated == wLen) {
                        break; //go to the next item
                    }
                }
            }
        }

    }

    for (int jCol = jTestCols; jCol < 256; jCol++) {
        PackedVecm[jCol] = 0;
        PackedVecl[jCol] = 0;
        unpackedVec[jCol]=0;
    }
}

void generate_rand_Z3_packed_Vec_4(uint64_t PackedVecm[4], uint64_t PackedVecl[4], uint64_t unpackedVec[256], std::mt19937 &generator)
{
    //for each word of the column, as we have 81 columns, so we need two 64-bit words for each
    for (int jCol = 0; jCol < 4; jCol++) {
        //go over the columns
            //fill the numbers for each row

            PackedVecm[jCol]=0;
            PackedVecl[jCol]=0;
            unpackedVec[jCol] = 0;

            int nBitsGenerated = 0;
            while (nBitsGenerated < wLen)  //we need two words for each column in the two MSB and LSB matrices we are filling
            {

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
                        PackedVecm[jCol] |= (bit1 << nBitsGenerated);
                        PackedVecl[jCol] |= (bit2 << nBitsGenerated);
                        unpackedVec[jCol*wLen+nBitsGenerated]=(bit1<<1 | bit2);
                        nBitsGenerated++;
                        //if we reached the end of the random matrix word, we need to go to the next word. We will then generate a new random word to fill it
                        if (nBitsGenerated == wLen) {
                            break; //go to the next item
                        }
                    }
                }
            }



        }
}




void generate_test_packed_sqMat_4(uint64_t mat[4][256], std::mt19937 &generator)
{
    for(int i = 0; i < 4; i++)
        for (int j = 0; j < 256; j++)
        {
            mat[i][j] = 0;
        }

    //number of bits that are non-zero
    int lBitTest = 1;
    int iWordTest = 1; //number of words that are non-zero
    int jColTest = 1;

    //srand(time(NULL));
    for(int i = 0; i < iWordTest; i++)
    {
        for (int j = 0; j < jColTest; j++)

            for (int l = 0; l < lBitTest; l++)
            //Call the generator that generate a 64-but word each time
                mat[i][j] |= (generator()& 1) << l;
    }
}
void generate_rand_packed_sqMat_4(uint64_t mat[4][256], std::mt19937 &generator)
{

    for(int j = 0; j < 256; j++)
        for(int i = 0; i < 4; i++)
        {
            //Call the generator that generate a 64-but word each time
            mat[i][j] = generator();
        }
}



/*
 * generate a random packed vector
 */




