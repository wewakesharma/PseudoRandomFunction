//
// Created by Vivek Sharma on 7/13/20.
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
#include "dmweakPRF.h"
#include "pi23protpacked.h"


using namespace std;
//declare the variables
uint64_t X, Rx, B, Rb;//packed version of values
uint64_t A, Ra;
uint64_t Z; //Ra*Rx + Rb
uint64_t poly_eval_2PC;
uint64_t out;
uint64_t RaRx;
uint64_t global_res;//stores result of AX+B evaluation(for testing)
uint64_t Ma,Mb;
uint64_t Ra_Mx, Ma_X, Mx;
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
    Ra = generator();
    Rb = generator();
    Rx = generator();
    Z = (Ra & Rx) ^ Rb;

    //PARTY2=================> generates random values for X and Rx.
    X = generator();
    Mx = X ^ Rx;

    //PARTY 1=================> generates A, B; while Ra and Rb are already generated in preprocessing
    A = generator();
    B = generator();
    Ma = A ^ Ra;
    Ra_Mx = Ra & Mx;
    Mb = Ra_Mx ^ B ^ Rb;

    Ma_X = Ma & X;
    out = Ma_X ^ Mb ^ Z;

    global_res = (A & X) ^ B;

    //Printing out the result for direct comparison.
    cout<<endl<<"Out"<<"\t"<<"global_res"<<endl;
    cout<<out<<"\t"<<global_res<<endl;




    return 0;
}