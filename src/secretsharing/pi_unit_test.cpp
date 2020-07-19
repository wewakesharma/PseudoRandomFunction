//
// Created by Vivek Sharma on 7/13/20.
//
#include <iostream>
#include <random>
#include <bitset>
#include <cmath>
#include <chrono>
#include <ctime>
#include <cstdlib>

#include "utils.h"
#include "pi23protpacked.h"
#include "pi_unit_test.h"
#include "dmweakPRFpacked.h"

using namespace std;
//==============================For Unit Testing of first three functions===================
void poly_eval_global(uint64_t A[4][256], uint64_t X[4], uint64_t B[4], uint64_t global_res[4])//AX+B evaluation
{
    uint64_t A_X[4];//intermediate polynomial evaluation, the term representation of A*X
    wordPackedVecMatMult(A,X,A_X);
    for(int cnt = 0; cnt < 4; cnt++)
        global_res[cnt] = A_X[cnt] ^ B[cnt];
}

void poly_eval_test(uint64_t out[4], uint64_t global_res[4])
{
    int test_flag = 0;
    for(int i =0;i<4;i++)
    {
        if(out[i] != global_res[i])
        {
            test_flag = 1;
            break;
        }

    }
    if(test_flag == 0)
        cout<<"Test Passed";
    else
        cout<<"Test failed";
}

/*
 * generate_msb_lsb_Z3() takes four arguments each of size 4.
 * The first parameter is generated Z3 value {0,1,2},
 * Following two parameters are its msb and lsb value respectively
 * The last parameter is the engine to generate the random bits.
 */
void generate_bits_4(uint64_t x_bits[4], std::mt19937 &generator)
{
    for(int i = 0; i < 4; i++)
    {
        x_bits[i] = generator() & 1;
    }
}
void generate_msb_lsb_Z3(uint64_t z3_value[4], uint64_t z3_msb[4], uint64_t z3_lsb[4], std::mt19937 &generator)
{
    //uint64_t rand_num = generator();
    int cnt = 0;
    uint64_t rand_num;
    while(cnt<4)
    {
         rand_num = generator()&3;
         if(rand_num < 3)//number should be 0,1, or 2
         {
             z3_value[cnt] = rand_num;
             z3_msb[cnt] = (rand_num>>1) & 1;  //AND with 10
             z3_lsb[cnt] = rand_num & 1; //AND with 01
             cout<<z3_value[cnt]<<"\t"<<z3_msb[cnt]<<"\t"<<z3_lsb[cnt]<<endl;
             cnt++;
         }
    }
}
