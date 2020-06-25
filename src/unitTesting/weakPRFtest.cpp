//
// Created by Vivek Sharma on 6/24/20.
//

#include <iostream>
#include <random>
#include <bitset>
#include <cmath>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include "weakPRFtest.h"

using namespace std;

//int wLen = 64;

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
        cout<<"***Phase 1 Test Passed***"<<endl;
    /* DEBUGGING
    cout<<endl<<"Printing the output of phase 1"<<endl;
    for(int i = 0; i< 256; i++)
    {
        cout<<pack_temp_out[i]<<"\t"<<naive_out_p1[i]<<endl;
    }*/
}

void merge_out_LM(uint64_t (&outM)[2], uint64_t (&outL)[2], uint64_t (&p3_out)[81])
{
    uint64_t bit_msb, bit_lsb, z3_bit;
    uint64_t msb[2], lsb[2];
    int row_limit = 81;
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
        if (naive_out_p3[row_count] != p3_out[row_count])
        {
            cout <<"Something is wrong in index " << row_count << endl;
            phase3_flag = 1;
            break;
        }
    }
    if(phase3_flag == 1)
    {
        cout<<"The computational outputs using naive and wordpacking DID NOT match, the function is NOT correct ==========> ERROR"<<endl; //prints if the break statement is not executed
        cout<<"!!!-Exiting the testing phase-!!!"<<endl;
    }
    else
        cout<<"***Phase 3 Test Passed***"<<endl;
    /*cout<<endl<<"Printing both output of phase3";
     DEBUGGING CODE
    for(int i = 0;i<81;i++)
    {
        cout<<naive_out_p3[i]<<"\t"<<p3_out[i]<<endl;
    }*/
}

void InnerProdMul2_test(uint64_t (&outVec)[84], uint64_t (&naive_out_p3)[81])
{
    bool test_flag = 0;
    //cout<<endl<<"outVec \t p3_out"<<endl;
    cout<<endl<<"Initializing unit testing for phase 3 (INNERPRODMUL2)==========>   O.K."<<endl;
    for(int i = 0; i < 81; i++)
    {
        if(outVec[i] != naive_out_p3[i])
        {
            test_flag = 1;
            break;
        }
    }
    if(test_flag == 1)
        cout<<"!!!-TEST FAILED-!!! The InnerProdMul2 test failed, please check the function";
    else
        cout<<"***Phase 3(InnerProdMul2) passed***"<<endl;
}

void InnerProdMul_test(uint64_t (&outVec)[84], uint64_t (&naive_out_p3)[81])
{
    bool test_flag = 0;
    //cout<<endl<<"outVec \t p3_out"<<endl;
    cout<<endl<<"Initializing unit testing for phase 3 (INNERPRODMUL)==========>   O.K."<<endl;
    for(int i = 0; i < 81; i++)
    {
        if(outVec[i] != naive_out_p3[i])
        {
            test_flag = 1;
            break;
        }
    }
    if(test_flag == 1)
        cout<<"!!!-TEST FAILED-!!! The InnerProdMul test failed, please check the function";
    else
        cout<<"***Phase 3(InnerProdMul) passed***"<<endl;
}

