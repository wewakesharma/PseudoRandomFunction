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
#include "utilspacked.h"
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
    cout<<"AX+B polynomial evaluation test status========>";
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
/*void generate_bits_4(uint64_t x_bits[4], std::mt19937 &generator)
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
}*/

//Debugging code
void OTPreproc_debug(uint64_t ram[4],uint64_t ral[4],uint64_t rbm[4],uint64_t rbl[4],uint64_t rx[4],uint64_t zm[4],uint64_t zl[4],std::mt19937 &generator)
{
    uint64_t unpackedVecra[256], unpackedVecrb[256];
    generate_test_Z3_packed_Vec_4(ram,ral,unpackedVecra, generator);
    generate_test_Z3_packed_Vec_4(rbm,rbl, unpackedVecrb, generator);

    generate_test_packed_vector_4(rx, generator);

    for (int i = 0; i < 4; i++) {
        zm[i] = (ram[i] & rx[i]) ^ (rbm[i] & (~rx[i]));
        zl[i] = (ral[i] & rx[i]) ^ (rbl[i] & (~rx[i]));
    }
}

void OT_test(uint64_t wm[4],uint64_t wl[4],uint64_t X[4],uint64_t r0m[4],uint64_t r0l[4],uint64_t r1m[4],uint64_t r1l[4])
{
    //cout<<endl<<"Printing Wm and Wl"<<endl;
    uint64_t X_bit, wm_bit, wl_bit, r0m_bit, r0l_bit, r1m_bit, r1l_bit;
    bool test_flag = 1;//test pass
    bool mismatch_x0 = 0;//mismatch when x = 0
    bool mismatch_x1 = 0;//mismatch when x = 1


    for(int i = 0; i < 1; i++)
    {
        uint64_t wm_word = wm[i];
        uint64_t wl_word = wl[i];
        uint64_t X_word = X[i];
        uint64_t r0m_word = r0m[i];
        uint64_t r0l_word = r0l[i];
        uint64_t r1m_word = r1m[i];
        uint64_t r1l_word = r1l[i];
        for (int jBit = 0; jBit < 64; jBit++) {
            //TODO: EXAMINE EACH BIT SEPARATELY OF WWordm, WWordl
            //and see that it matches either r0m, r0l or r1m, r1l
            //cepending on the value of the XWord'i bith
            X_bit = (X_word >> jBit) & 1;
            wm_bit = (wm_word >> jBit) & 1;
            wl_bit = (wl_word >> jBit) & 1;
            r0m_bit = (r0m_word >> jBit) & 1;
            r0l_bit = (r0l_word >> jBit) & 1;
            r1m_bit = (r1m_word >> jBit) & 1;
            r1l_bit = (r1l_word >> jBit) & 1;
            if((X_bit == 0) & ((wl_bit != r0l_bit) | (wm_bit != r0m_bit)))//Wl,Wm didn't match with r0l and r0m respectively
            {
                mismatch_x0 = 1;//error when x=0
                cout<<"fails at (X=0)"<<jBit;
                break;//breaks out of inner loop
            }
            if((X_bit == 1) & ((wl_bit != r1l_bit) | (wm_bit != r1m_bit)))//Wl,Wm didn't match with r1l and r1m respectively
            {
                mismatch_x1 = 1;//error when x=0
                cout<<"fails at (X = 1)"<<jBit;
                break;//breaks out of inner loop
            }
        }
        if((mismatch_x0 == 1) | (mismatch_x1 == 1)){
            test_flag = 0;
            break;//breaks out of the outer loop
        }
    }
    cout<<endl<<"OT test status========";
    if(test_flag == 0)
        cout<<"Test fails";
    else
        cout<<"Test passed";
}