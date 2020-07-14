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

#include "include/utils.h"
#include "include/pi23protpacked.h"
#include "pi_unit_test.h"

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