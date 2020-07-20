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
#include "dmweakPRFpacked.h"
#include "pi23protpacked.h"
#include "pi_unit_test.h"


using namespace std;
//declare the variables

//uint64_t poly_eval_2PC[4];
//uint64_t RaRx[4];
uint64_t global_res[4];//stores result of AX+B evaluation(for testing)



int main()
{
    uint64_t X[4], Rx[4], B[4], Rb[4];//packed version of values
    uint64_t A[4][256], Ra[4][256];
    uint64_t Z[4]; //Ra*Rx + Rb
    uint64_t out[4];

    //uint64_t mxbitGlobal;
    //uint64_t MxGlobal[4];
    //uint64_t m0Global, m1Global;
    uint64_t rx[4]; //number generated in OTPreproc
    //uint64_t raGlobal[4], rbGlobal[4],r0Global[4], r1Global[4];//ra, rb, r1, r0 values in Z3
    uint64_t r0l[4],r0m[4],r1l[4],r1m[4],ral[4],ram[4],rbl[4],rbm[4];//LSB and MSB of r0, r1, ra, rb
    //uint64_t Zl[4], Zm[4];
    uint64_t wl[4], wm[4], wGlobal[4];
    uint64_t r0unpck[256],r1unpck[256];
    uint64_t zm[4],zl[4];

    unsigned seed = 7;    // std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);

    //PREPROCESSING==> generating Ra, Rb and Rx and computing Z = RaRx + Rb
    PreProcPackedGenVals(Ra,Rb,Rx,Z,generator);

    //PARTY2==> generates random values for X and Rx.
    //generate_rand_packed_vector_4(X,generator);
    generate_test_packed_vector_4(X,generator);
    AXplusB_P2PackedPart1(X,Rx,Z);

    //PARTY 1==> generates A, B; while Ra and Rb are already generated in preprocessing
    generate_rand_packed_sqMat_4(A,generator);
    generate_rand_packed_vector_4(B,generator);
    AXplusB_P1Packed(A,B,Ra,Rb);

    AXplusB_P2PackedPart2(X,Rx,Z,out);//evaluation of polynomial using Ma*X + Mb + Z

    poly_eval_global(A,X,B,global_res);    //evaluation of polynomial using Ax+B
    poly_eval_test(out, global_res); //comparing both the outputs to compare the result
    //Test Complete - AX +B polynomial evaluation

    //Unit Test - 2 =========================OT evaluation===========================
    //Receiver Part 1-generate bits x and Rx which are bits{0,1}
    //X is already generated and Rx will be fetched through OT Preprocessing
    OTPreproc_debug(ram,ral,rbm,rbl,rx,zm,zl,generator);//DEBUG
    //OT_fetch_preprocessed_values(ram,ral,rbm,rbl,rx,zm,zl);

    //setting X=0 for debugging


    OTZ3_R_Part1Packed(X, rx);//X and rx

    //Sender
    //generate r0, r0l and r0m
    //generate_rand_Z3_packed_Vec_4(r0m,r0l,r0unpck,generator);
    generate_rand_Z3_packed_Vec_4(r0m,r0l,r0unpck,generator);//DEBUG

    //generate r1, r1l and r1m
    //generate_rand_Z3_packed_Vec_4(r1m,r1l,r1unpck,generator);
    generate_rand_Z3_packed_Vec_4(r1m,r1l,r1unpck,generator);//DEBUG
    OTZ3_S_Packed(r0m,r0l,r1m,r1l,ram,ral,rbm,rbl);//doesn't need to do anything except calling the function

    //Receiver Part 2
    OTZ3_R_Part2Packed(rx,zm,zl,wm,wl);

    //Test function defined in pi_unit_test.cpp
    OT_test(wm,wl,X,r0m,r0l,r1m,r1l);

    ////Unit Test - 3 =========================SC test===========================
/*
    //Receiver Part 1
    uint64_t Y2[4];
    sc23_p2Part1Packed(Y2,generator);

    //Sender
    uint64_t y1[4], vm[4],vl[4];
    sc23_p1Packed(y1,vm,vl,generator);

    //Receiver Part 2*/
    return 0;
}

