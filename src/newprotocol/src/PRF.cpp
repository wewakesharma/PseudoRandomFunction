// Copyright (C) 2020 Tzipora Halevi, MIT license - see LICENSE file
/** Toeplitz-by-x.cpp
 *  - implementing a protocol for A*x (aka A*x+b) mod 2
 *    A is a Toeplitz matrix
 */
#include <iostream>
#include <cassert>
#include "packedMod2.hpp"
#include "Toeplitz-by-x.hpp"
#include "OT.hpp"
#include "mains.hpp"
#include "Timing.hpp"
#include <chrono>
#include "packedMod3.hpp"
#include "utils.hpp"
#include "lookup_functions.h"

// in Toeplitz-by-x.hpp
// #define N_ROWS 256
// #define N_COLS 256


#ifdef TEST_PRF
using namespace std;


long timerPRF = 0;  //times the entire DM wPRF

long timerAxpBP1 = 0;   //times the party 1 round 1 of DM wPRF
long timerAxpBP2 = 0;   //times the party 1 round 1 of DM wPRF

long timerOTP1 = 0;     //times the party 1 phase 2
long timerOTP2 = 0;

long timerSCP1 = 0;
long timerSCP2 = 0;

long timer_phase3 = 0;  //total time to complete phase 3(matByVec)
long timer_phase31 = 0;     //time required to complete phase 3 by party 1
long timer_phase32 = 0;     //time required to complete phase 3 by party 2

long timer_phase3_lookup = 0;  //total time to complete phase 3(using lookup table)
long timer_phase31_lookup = 0;
long timer_phase32_lookup = 0;

#endif
// A place to store the results from pre-processing

static std::vector< std::vector<uint64_t> > rAs;
static std::vector< PackedZ2<N_ROWS> > rbs, rzs;
static std::vector< PackedZ2<N_COLS> > rxs;
PackedZ3<81> out_dummy;

//This is the centralized implementation, acting as the base version for packing. FOr lookup implementation go to test_packed_central_lookup.cpp

  void PRF_packed_centralized(std::vector<uint64_t>& K1, PackedZ2<N_COLS>& x1, std::vector<uint64_t>& K2,
                            PackedZ2<N_COLS>& x2, std::vector< PackedZ3<81> >& Rmat,  PackedZ3<81>& outZ3, int nTimes)
{
    #ifdef DEBUG
        cout<<endl<<"PRF.cpp/PRF_packed_centralized_res_compare()"<<endl;
    #endif

    //1.perform X = x1+ x2 (on vectors)
    PackedZ2<N_COLS> X = x1; //declare a variable
    X ^= x2;    //x = x1 + x2

    //2.perform K = k1 + k2 (on matrix)
    std::vector<uint64_t> K(toeplitzWords);
    for (int i = 0; i < K1.size(); i++)
    {
        K[i] = K1[i] ^ K2[i];
    }
    #ifdef PRINT_VAL
        cout<<"x "<<X<<endl;
        cout<<"K "<<K<<endl;
    #endif

    PackedZ2<N_COLS> outKX;
    outKX.toeplitzByVec(K,X);
    #ifdef PRINT_VAL
        cout<<"outKX "<<outKX<<endl;
    #endif

    std::vector<unsigned int> outKX_unsgn;//unsigned int of outKX i.e.(K*x)
    outKX.toArray(outKX_unsgn);

    PackedZ3<256> outKX_Z3;//packed Z3
    //PackedZ3<81> outZ3;//final Z3 output
    outKX_Z3.fromArray(outKX_unsgn);//converting unsigned int to PackedZ2

    #ifdef PRINT_VAL
        cout<<"Both outKX and outKX_Z3 must be same"<<endl;
        cout<<"outKX_Z3 "<<outKX_Z3<<endl;
    #endif

    //IMPORTANT FOR RUNNING NEW PROTOCOL WITHOUT LOOKUP TABLE: COMMENT IT IF LOOKUP IS USED
    outZ3.matByVec(Rmat,outKX_Z3);//output of randmat*K*x
    #ifdef PRINT_VAL
        cout<<"outZ3 "<<outZ3<<endl;
    #endif
}


#ifdef TEST_PRF
void display_timing()
{
    using Clock = std::chrono::system_clock;
    using Duration = Clock::duration;
    //std::cout << Duration::period::num << " , " << Duration::period::den << '\n';
    float time_unit_multiplier = 1;
    if(Duration::period::den == 1000000000)
        time_unit_multiplier = 0.001; //make nanosecond to microsecond
    else if(Duration::period::den == 1000000)
        time_unit_multiplier = 1;   //keep the unit as microsecond

    cout<<endl<<"AX + B execution time "<<endl;
    cout<<"Party 1: " << (timerAxpBP1 * time_unit_multiplier)<<" microseconds"<<endl;
    cout<<"Party 2: "<<(timerAxpBP2 * time_unit_multiplier)<<" microseconds"<<endl;
    std::cout<<"Time to execute phase 1 (K*X): "<<
             ((timerAxpBP1 + timerAxpBP2) * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"Number of rounds per second for phase 1: "<<(1000/((timerAxpBP1 + timerAxpBP2)*time_unit_multiplier)*1000000)<<std::endl;
    std::cout<<"====================================================="<<std::endl;
    std::cout<<std::endl<<"Share Conversion execution time "<<std::endl;
    std::cout<<"Party 1: " << (timerSCP1* time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"Party 2: "<<(timerSCP2 * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"Time to execute phase 2(Share Conversion): "<<
             ((timerSCP1 + timerSCP2) * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"Number of rounds per second for phase 2: "<<(1000/((timerSCP1 + timerSCP2) * time_unit_multiplier)*1000000)<<std::endl;
    std::cout<<"====================================================="<<std::endl;
#ifndef TEST_PRF_LOOKUP
    timer_phase3 = std::max(timer_phase31,timer_phase32);       //taking maximum of both phases to emulate simultaneous execution
    std::cout<<"\nphase 3 party 1: "<<(timer_phase31 * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"\nphase 3 party 2: "<<(timer_phase32 * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"\nTime to execute phase 3: "<<
             (timer_phase3 * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"Number of rounds per second for phase 3: "<<(1000/(timer_phase3*time_unit_multiplier)*1000000)<<std::endl;
    timerPRF = (timerAxpBP1 + timerAxpBP2) + (timerSCP1 + timerSCP2) + timer_phase3;
#endif

#ifdef TEST_PRF_LOOKUP
    timer_phase3_lookup = std::max(timer_phase31_lookup,timer_phase32_lookup);       //taking maximum of both phases to emulate simultaneous execution
    std::cout<<"\nphase 3 party 1: "<<(timer_phase31_lookup * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"\nphase 3 party 2: "<<(timer_phase32_lookup * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"\nTime to execute phase 3: "<<
             (timer_phase3_lookup * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"Number of rounds per second for phase 3: "<<(1000/(timer_phase3_lookup*time_unit_multiplier)*1000000)<<std::endl;
    timerPRF = (timerAxpBP1 + timerAxpBP2) + (timerSCP1 + timerSCP2) + timer_phase3_lookup;
#endif

    std::cout<<"====================================================="<<std::endl;

    std::cout<<"\nTime to execute entire PRF protocol: "<<
             (timerPRF * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"Number of rounds per second for entire PRF: "<<(1000/(timerPRF*time_unit_multiplier)*1000000)<<std::endl;

}



void PRF_DM(vector<uint64_t>& K1, PackedZ2<N_COLS>& x1, vector<uint64_t>& K2,
        PackedZ2<N_COLS>& x2, std::vector< PackedZ3<81> >& Rmat, PackedZ3<81>& out1Z3,
        PackedZ3<81>& out2Z3, int nRuns)
{
#ifdef TEST_PRF_LOOKUP
    //local copy of lookup table and Rmat16
    std::vector<std::vector<PackedZ3<81> > > Rmat16_prf(16); //Rmat16_prf vector of size 16 X 16 X 81 (GLOBAL)
    std::vector<std::vector<PackedZ3<81> > > lookup_prf(16); //lookup table

    //reformat Rmat
    reformat_Rmat(Rmat16_prf, Rmat);        //convert (81 X 256) matrix to (16 x 16 X 81) matrix

    //Creation of Lookup table
    create_lookup_table(Rmat16_prf, lookup_prf);        //(a lookup table of 16 X 2^16) is generated
    std::cout<<"Lookup table created"<<std::endl;
#endif

    PackedZ2<N_ROWS> out1_A, out2_A, out1_B, out2_B;

    chrono::time_point<std::chrono::system_clock> start_p1,start_p2;
    //Timing declaration for lookup implementation in PRF phase 3
    std::chrono::time_point<std::chrono::system_clock> start_p31_lookup, start_p32_lookup;
    std::chrono::time_point<std::chrono::system_clock> start_p31, start_p32;
    std::chrono::time_point<std::chrono::system_clock> start_reformat, start_uselookup, start_subtract;



    for (int i = 0; i < nRuns; i++) {   //nRuns is the number of runs, for timing we keep it at 1000

        //NOTE: The timing for Toeplitz_partyx[_x] is within the function.
        //Start of phase 1:common for both without and with lookup

        // First run, a protocol for K1 times x2
        topelitz_Party2_1(x2, 2 * i);
        topelitz_Party1(out1_A, K1, 2 * i);
        topelitz_Party2_2(out1_B, x2, 2 * i);

        // Second run, a protocol for K2 times x1
        topelitz_Party2_1(x1, 2 * i + 1);
        topelitz_Party1(out2_B, K2, 2 * i + 1);
        topelitz_Party2_2(out2_A, x1, 2 * i + 1);

        start_p1 = chrono::system_clock::now();

        // Party1 computes locally K1 times x1, and adds to out1_A,out2_A
        out1_A.add(out2_A);           // out1 ^= out2
        out2_A.toeplitzByVec(K1, x1); // K1 times x1
        out1_A.add(out2_A);           // sum of all terms

        timerAxpBP1 += (chrono::system_clock::now() - start_p1).count();

        start_p2 = chrono::system_clock::now();

        // Party2 computes locally K2 times x2, and adds to out1_B,out2_N
        out1_B.add(out2_B);           // out1 ^= out2
        out2_B.toeplitzByVec(K2, x2); // K2 times x2
        out1_B.add(out2_B);           // sum of all terms

        timerAxpBP2 += (chrono::system_clock::now() - start_p2).count();
        //=============end of phase 1==================

        //============Start of Phase 2(common for both lookup and without lookup===============
        PackedZ2<N_SIZE> &y1 = out1_A;
        PackedZ2<N_SIZE> &y2 = out1_B;
        PackedZ3<N_SIZE> out1, out2;

        SC_Party2_1(y2, i);             //The implementation details for SC functions are within the function.
        SC_Party1(y1, out1, i);
        SC_Party2_2(y2, out2, i);
        //=============end of phase 2=================



#ifndef TEST_PRF_LOOKUP     //party 1 phase 3 without lookup implementation
        //===============party 1==================

        start_p31 = chrono::system_clock::now();
        out1Z3.matByVec(Rmat, out1); // compute matrix-by-vector multiply
        timer_phase31 += (std::chrono::system_clock::now() - start_p31).count();

        //===============party 2==================

        start_p32 = chrono::system_clock::now();
        out2Z3.matByVec(Rmat, out2);    //compute matrix-by-vector multiply
        timer_phase32 += (std::chrono::system_clock::now() - start_p32).count();
#endif

#ifdef TEST_PRF_LOOKUP  //party 1 phase 3 with lookup implementation

        start_p31_lookup = chrono::system_clock::now();//starting the lookup timing for phase 3
        //===============party 1==================
        usedLookupTable(out1Z3,out1,lookup_prf);

        timer_phase31_lookup += (std::chrono::system_clock::now() - start_p31_lookup).count();//gives output for phase 3 party 1 using lookup

        //===============party 2==================

        start_p32_lookup = chrono::system_clock::now();

        usedLookupTable(out2Z3,out2,lookup_prf);

        timer_phase32_lookup += (std::chrono::system_clock::now() - start_p32_lookup).count();//gives output for phase 3 party 2 using lookup
#endif

        out_dummy += out1Z3;
        out_dummy += out2Z3;        //creating and assigning output to dummy variable so that the timing don't get skipped
    }
}

/*
 * PRF_DM includes pre-processing
 */

void PRF_DM_wpreproc(unsigned int nTimes,  int nRuns, int nStages) {

    randomWord(1); // use seed=1
    vector<uint64_t> K1(toeplitzWords), K2(toeplitzWords);  //Key to the protocol
    PackedZ2<N_COLS> x1, x2;            //input of the protocol
    PackedZ3<81> outZ3;         //output of packed_centralized implementation used for testing

    PackedZ2<N_ROWS> out1_A, out2_A, out1_B, out2_B;

    initGlobals();      //initialize some global variables

    std::vector<PackedZ3<81> > Rmat(256); // generate a 81x256 matrix

    //randomize the matrix
    for (auto &col : Rmat) // iterate over the columns
        col.randomize();

    preProc_Toeplitz_by_x(nRuns * 2); // pre-processing for two runs
    preProc_OT(nRuns); //preprocess for OT, generate ra, rn, rx, and z

    // Choose random K1, K2, x1, x2, we will be computing
    // (K1 xor K2) \times (x1 xor x2)

    //TODO: write randomize Toeplitzß

    for (auto &w : K1) w = randomWord();
    K1[K1.size() - 1] &= topelitzMask; // turn off extra bits at the end

    for (auto &w : K2) w = randomWord();
    K2[K2.size() - 1] &= topelitzMask; // turn off extra bits at the end

    x1.randomize();     //setting random input for party 1
    x2.randomize();     //setting random input for party 2

    PackedZ3<81> out1Z3;                     // party 1's protocol output
    PackedZ3<81> out2Z3;                     // party 2's protocol output

    PackedZ3<81> outSum;            //final output of entire dark matter wPRF
    outSum.reset();

    std::cout<<" Number of Runs: "<<nRuns<<std::endl;

    PRF_DM(K1, x1, K2, x2, Rmat, out1Z3, out2Z3, nRuns); // Calling the main function

    outSum = out1Z3;
    outSum += out2Z3;   //computing the output of entire PRF by adding the shares of output by both the parties

    cout << "Output of PRF Dark matter, outSum=" << outSum << endl;
    PRF_packed_centralized(K1,x1,K2,x2,Rmat,outZ3,1);   //centralized implementation for testing
    cout << "Output of PRF centralized implementation, outZ3=" << outZ3 << endl;

    if(outSum == outZ3) //outZ3 is the output of centralized and outSum is output of DM wPRF protocol.
        std::cout<<std::endl<<"Test passed"<<std::endl;
    else
        std::cout<<std::endl<<"Test failed"<<std::endl;
    display_timing();       //displays the timing and number of rounds in each phase of the protocol

}
#endif
