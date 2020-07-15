//
// Created by Vivek Sharma on 7/6/20.
//

/*
 * pi protocol testing using word packing.
 */
//include the system defined header files
#include <iostream>
#include <random>
#include <bitset>
#include <cmath>
#include <chrono>
#include <ctime>
#include <cstdlib>

//include the user defined header files

#include "utils.h"
#include "pi23protpacked.h"
#include "pi_unit_test.h"


using namespace std;
//declare the variables
uint64_t X[4], Rx[4], B[4], Rb[4];//packed version of values
uint64_t A[4][256], Ra[4][256];
uint64_t Z[4]; //Ra*Rx + Rb
uint64_t poly_eval_2PC[4];
uint64_t out[4];
uint64_t RaRx[4];
uint64_t global_res[4];//stores result of AX+B evaluation(for testing)

//uint64_t mxbitGlobal;
//uint64_t MxGlobal[4];
//uint64_t m0Global, m1Global;
//uint64_t raGlobal, rbGlobal;
//uint64_t rxGlobal, zGlobal;


int main()
{
    unsigned seed = 7;    // std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);

    //PREPROCESSING==========> generating Ra, Rb and Rx and computing Z = RaRx + Rb
    PreProcPackedGenVals(Ra,Rb,Rx,Z,generator);

    //PARTY2=================> generates random values for X and Rx.
    generate_rand_packed_vector_4(X,generator);
    AXplusB_P2PackedPart1(X,Rx,Z);

    //PARTY 1=================> generates A, B; while Ra and Rb are already generated in preprocessing
    generate_rand_packed_sqMat_4(A,generator);
    generate_rand_packed_vector_4(B,generator);
    AXplusB_P1Packed(A,B,Ra,Rb);

    AXplusB_P2PackedPart2(X,Rx,Z,out);//evaluation of polynomial using Ma*X + Mb + Z

    poly_eval_global(A,X,B,global_res);    //evaluation of polynomial using Ax+B
    poly_eval_test(out, global_res); //comparing both the outputs to compare the result
    //Printing out the result for direct comparison.
    cout<<endl<<"Out"<<"\t"<<"global_res"<<endl;
    for(int i = 0; i < 4; i++)
    {
        cout<<out[i]<<"\t"<<global_res[i]<<endl;
    }




    return 0;
}

