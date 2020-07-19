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

    uint64_t mxbitGlobal;
    uint64_t MxGlobal[4];
    uint64_t m0Global, m1Global;
    uint64_t rx[4], z[4], x[4];
    uint64_t raGlobal[4], rbGlobal[4],r0Global[4], r1Global[4];//ra, rb, r1, r0 values in Z3
    uint64_t r0l[4],r0m[4],r1l[4],r1m[4],ral[4],ram[4],rbl[4],rbm[4];//LSB and MSB of r0, r1, ra, rb
    //uint64_t Zl[4], Zm[4];
    uint64_t Wl[4], Wm[4], WGlobal[4];
    uint64_t r0unpck[256],r1unpck[256],raunpck[256],rbunpck[256],Zunpck[256];

    unsigned seed = 7;    // std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);

    //PREPROCESSING==> generating Ra, Rb and Rx and computing Z = RaRx + Rb
    PreProcPackedGenVals(Ra,Rb,Rx,Z,generator);

    //PARTY2==> generates random values for X and Rx.
    generate_rand_packed_vector_4(X,generator);
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
    //Receiver Part 1-generate bits x and rx which are bits{0,1}
    //generate_bits_4(x,generator);
    //generate_bits_4(rx,generator);
    OTPreproc(generator);
    rxOTGlobal;
    OTZ3_R_Part1Packed(X, Rx);

    //Sender
    //generate r0, r0l and r0m
    generate_rand_Z3_packed_Vec_4(r0m,r0l,r0unpck,generator);
    //generate r1, r1l and r1m
    generate_rand_Z3_packed_Vec_4(r1m,r1l,r1unpck,generator);
    //generate ra, ral and ram
    generate_rand_Z3_packed_Vec_4(ram,ral,raunpck,generator);
    //generate rb, rbl and rbm
    generate_rand_Z3_packed_Vec_4(rbm,rbl,rbunpck,generator);

    OTZ3_S_Packed(r0m,r0l,r1m,r1l,ram,ral,rbm,rbl);//doesn't need to do anything except calling the function

    //Receiver Part 2
    //generate_rand_Z3_packed_Vec_4(Zm,Zl,Zunpck,generator);
    //generate_msb_lsb_Z3(z,Zm,Zl,generator);//generate zGlobal in Z3 and store their bit representation in Zm and Zl
    OTZ3_R_Part2Packed(Rx,Zm,Zl,Wm,Wl);

    cout<<endl<<"Printing Wm and Wl"<<endl;
    uint64_t X_bit, Wm_bit, Wl_bit, r0m_bit, r0l_bit, r1m_bit, r1l_bit;
    bool test_flag = 1;//test pass
    bool mismatch_x0 = 0;//mismatch when x = 0
    bool mismatch_x1 = 0;//mismatch when x = 1

    for(int i = 0; i < 1; i++)
    {
        uint64_t Wm_word = Wm[i];
        uint64_t Wl_word = Wl[i];
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
            Wm_bit = (Wm_word >> jBit) & 1;
            Wl_bit = (Wl_word >> jBit) & 1;
            r0m_bit = (r0m_word >> jBit) & 1;
            r0l_bit = (r0l_word >> jBit) & 1;
            r1m_bit = (r1m_word >> jBit) & 1;
            r1l_bit = (r1l_word >> jBit) & 1;
            if((X_bit == 0) & ((Wl_bit != r0l_bit) | (Wm_bit != r0m_bit)))//Wl,Wm didn't match with r0l and r0m respectively
            {
                mismatch_x0 = 1;//error when x=0
                break;//breaks out of inner loop
            }
            if((X_bit == 1) & ((Wl_bit != r1l_bit) | (Wm_bit != r1m_bit)))//Wl,Wm didn't match with r1l and r1m respectively
            {
                mismatch_x1 = 1;//error when x=0
                break;//breaks out of inner loop
            }
        }
        if((mismatch_x0 == 1) | (mismatch_x1 == 1)){
            test_flag = 0;
            break;//breaks out of the outer loop
        }
    }
    if(test_flag == 0)
        cout<<"Test fails";
    else
        cout<<"Test passed";
    return 0;
}

