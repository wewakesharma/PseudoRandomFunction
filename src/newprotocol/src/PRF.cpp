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

long timer_phase3 = 0;  //total time to complete phase 3(matByVec)
long timer_phase31 = 0;     //time required to complete phase 3 by party 1
long timer_phase32 = 0;     //time required to complete phase 3 by party 2
long timer_final_output = 0; //final timer to compute addition of output of two shares.

long timer_phase3_lookup = 0;  //total time to complete phase 3(using lookup table)
long timer_phase31_lookup = 0;
long timer_phase32_lookup = 0;

#endif

//global variables to emulate storage in file.
PackedZ2<N_ROWS> global_out1_A, global_out1_B, global_out2_A, global_out2_B;
PackedZ3<N_SIZE> global_SC_out1, global_SC_out2;
PackedZ3<81> out_dummy;

//============================================================================
//This is the centralized implementation, acting as the base version for packing.
// For lookup implementation go to test_packed_central_lookup.cpp

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
//==============================================================

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
    timer_phase3 = std::max(timer_phase31,timer_phase32) + timer_final_output;       //taking maximum of both phases to emulate simultaneous execution
    std::cout<<"\nphase 3 party 1: "<<(timer_phase31 * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"\nphase 3 party 2: "<<(timer_phase32 * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"\nTime to execute phase 3: "<<
             (timer_phase3 * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"Number of rounds per second for phase 3: "<<(1000/(timer_phase3*time_unit_multiplier)*1000000)<<std::endl;
    timerPRF = (timerAxpBP1 + timerAxpBP2) + (timerSCP1 + timerSCP2) + timer_phase3;
#endif

#ifdef TEST_PRF_LOOKUP
    timer_phase3_lookup = std::max(timer_phase31_lookup,timer_phase32_lookup) + timer_final_output;       //taking maximum of both phases to emulate simultaneous execution
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

//=============================Modular Code========================

void DM_preprocess(int nTimes)
{
    initGlobals();      //initialize some global variables
    preProc_Toeplitz_by_x(nTimes * 2); // pre-processing for two runs
    preProc_OT(nTimes); //preprocess for OT, generate ra, rn, rx, and z
}

void set_input(std::vector<uint64_t>& K1,PackedZ2<N_COLS>& x1,
                     std::vector<uint64_t>& K2, PackedZ2<N_COLS>& x2, std::vector<PackedZ3<81> >& Rmat)
{
    for (auto &w : K1) w = randomWord();
    K1[K1.size() - 1] &= topelitzMask; // turn off extra bits at the end

    for (auto &w : K2) w = randomWord();
    K2[K2.size() - 1] &= topelitzMask; // turn off extra bits at the end

    x1.randomize();     //setting random input for party 1
    x2.randomize();     //setting random input for party 2

    for (auto &col : Rmat) // iterate over the columns
        col.randomize();
}

void setup_table(std::vector<std::vector<PackedZ3<81> > >& lookup_prf, std::vector<PackedZ3<81> >& Rmat)
{
    //local copy of lookup table and Rmat16
    std::vector<std::vector<PackedZ3<81> > > Rmat16_prf(16); //Rmat16_prf vector of size 16 X 16 X 81 (GLOBAL)

    //reformat Rmat
    reformat_Rmat(Rmat16_prf, Rmat);        //convert (81 X 256) matrix to (16 x 16 X 81) matrix

    //Creation of Lookup table
    create_lookup_table(Rmat16_prf, lookup_prf);        //(a lookup table of 16 X 2^16) is generated
    std::cout<<"Lookup table created"<<std::endl;
}

void party1_local_computation(std::vector<uint64_t>& K1,PackedZ2<N_COLS>& x1,
                              PackedZ2<N_ROWS>& out1_A, PackedZ2<N_ROWS>& out2_A)
{
    // Party1 computes locally K1 times x1, and adds to out1_A,out2_A
    out1_A.add(out2_A);           // out1 ^= out2
    out2_A.toeplitzByVec(K1, x1); // K1 times x1
    out1_A.add(out2_A);           // sum of all terms
}

void party2_local_computation( std::vector<uint64_t>& K2, PackedZ2<N_COLS>& x2,
                               PackedZ2<N_ROWS>& out1_B, PackedZ2<N_ROWS>& out2_B)
{
    // Party2 computes locally K2 times x2, and adds to out1_B,out2_N
    out1_B.add(out2_B);           // out1 ^= out2
    out2_B.toeplitzByVec(K2, x2); // K2 times x2
    out1_B.add(out2_B);           // sum of all terms
}

void round1(std::vector<uint64_t>& K1,PackedZ2<N_COLS>& x1, std::vector<uint64_t>& K2, PackedZ2<N_COLS>& x2,
            PackedZ2<N_ROWS>& out1_A, PackedZ2<N_ROWS>& out2_A, PackedZ2<N_ROWS>& out1_B, PackedZ2<N_ROWS>& out2_B, int nTimes)
{

    std::chrono::time_point<std::chrono::system_clock> start_p1,start_p2;

    for(int i = 0; i < nTimes; i++){
        // First run, a protocol for K1 times x2
        topelitz_Party2_1(x2, 2 * i);
        topelitz_Party1(out1_A, K1, 2 * i);
        topelitz_Party2_2(out1_B, x2, 2 * i);

        // Second run, a protocol for K2 times x1
        topelitz_Party2_1(x1, 2 * i + 1);
        topelitz_Party1(out2_B, K2, 2 * i + 1);
        topelitz_Party2_2(out2_A, x1, 2 * i + 1);
    }

    //start_p1 = chrono::system_clock::now();
    party1_local_computation(K1, x1, out1_A,out2_A);
    //timerAxpBP1 += (chrono::system_clock::now() - start_p1).count();

    //start_p2 = chrono::system_clock::now();
    party2_local_computation(K2, x2, out1_B,out2_B);
    //timerAxpBP2 += (chrono::system_clock::now() - start_p2).count();
}

void save_round1_output(PackedZ2<N_ROWS>& out1_A, PackedZ2<N_ROWS>& out1_B, PackedZ2<N_ROWS>& out2_A,PackedZ2<N_ROWS>& out2_B)
{
    global_out1_A = out1_A;
    global_out1_B = out1_B;
    global_out2_A = out2_A;
    global_out2_B = out2_B;
}

void fetch_round1_output(PackedZ2<N_ROWS>& out1_A, PackedZ2<N_ROWS>& out1_B, PackedZ2<N_ROWS>& out2_A, PackedZ2<N_ROWS>& out2_B)
{

    out1_A = global_out1_A;
    out1_B = global_out1_B;
    out2_A = global_out2_A;
    out2_B = global_out2_B;
}

void save_round2_output(PackedZ3<N_SIZE>& SC_out1, PackedZ3<N_SIZE>& SC_out2)
{
    global_SC_out1 = SC_out1;
    global_SC_out2 = SC_out2;
}

void fetch_round2_output(PackedZ3<N_SIZE>& SC_out1, PackedZ3<N_SIZE>& SC_out2)
{
    SC_out1 = global_SC_out1;
    SC_out2 = global_SC_out2;
}

void round2(PackedZ3<N_SIZE>& SC_out1, PackedZ3<N_SIZE>& SC_out2, PackedZ2<N_ROWS>& out1_A, PackedZ2<N_ROWS>& out2_A,
            PackedZ2<N_ROWS>& out1_B,PackedZ2<N_ROWS>& out2_B, int nTimes)
{
    PackedZ2<N_SIZE> &y1 = out1_A;
    PackedZ2<N_SIZE> &y2 = out1_B;

    for(int i = 0; i < nTimes;i++)
    {
        SC_Party2_1(y2, i);             //The implementation details for SC functions are within the function.
        SC_Party1(y1, SC_out1, i);
        SC_Party2_2(y2, SC_out2, i);
    }

}
void round3_party1_lookup(PackedZ3<81>& out1Z3, PackedZ3<N_SIZE>& SC_out1,std::vector<std::vector<PackedZ3<81> > >& lookup_prf)
{
    usedLookupTable(out1Z3,SC_out1,lookup_prf);
}

void round3_party2_lookup(PackedZ3<81>& out2Z3, PackedZ3<N_SIZE>& SC_out2,std::vector<std::vector<PackedZ3<81> > >& lookup_prf)
{
    usedLookupTable(out2Z3,SC_out2,lookup_prf);
}

void final_protocol_output(PackedZ3<81>& out_protocol_result, PackedZ3<81>& out1Z3, PackedZ3<81>& out2Z3)
{
    out_protocol_result = out1Z3;
    out_protocol_result += out2Z3;
}


void PRF_DM(vector<uint64_t>& K1, PackedZ2<N_COLS>& x1, vector<uint64_t>& K2,
        PackedZ2<N_COLS>& x2, std::vector< PackedZ3<81> >& Rmat, PackedZ3<81>& out1Z3,
        PackedZ3<81>& out2Z3, int nRuns, int nTimes)
{


#ifdef TEST_PRF_LOOKUP
    //local copy of lookup table and Rmat16
    std::vector<std::vector<PackedZ3<81> > > Rmat16_prf(16); //Rmat16_prf vector of size 16 X 16 X 81 (GLOBAL)
    std::vector<std::vector<PackedZ3<81> > > lookup_prf(16); //lookup table

    //setup lookup preprocessing
    setup_table(lookup_prf, Rmat);
    std::cout<<"Lookup table created"<<std::endl;
#endif

    PackedZ2<N_ROWS> out1_A, out2_A, out1_B, out2_B;
    PackedZ3<N_SIZE> SC_out1, SC_out2;        //phase 2 output
    PackedZ3<81> out_protocol_result;            //final output of entire dark matter wPRF
    out_protocol_result.reset();

    chrono::time_point<std::chrono::system_clock> start_p1,start_p2, start_final_output;
    //Timing declaration for lookup implementation in PRF phase 3
    std::chrono::time_point<std::chrono::system_clock> start_p31_lookup, start_p32_lookup;
    std::chrono::time_point<std::chrono::system_clock> start_p31, start_p32;//start timers for without lookup
    std::chrono::time_point<std::chrono::system_clock> start_round3_p1, start_round3_p2; //start timer with lookup
    std::chrono::time_point<std::chrono::system_clock> start_reformat, start_uselookup, start_subtract;

    /*For micro benchmarking purpose
    //run round 1 just once and save out1_A, out2_A, out1_B and out2_B in a file.
    round1(K1,x1,K2,x2,out1_A,out2_A,out1_B,out2_B,nTimes);

    //run round 2 just once and save out1_A, out2_A, out1_B and out2_B in a file.
    round2(SC_out1, SC_out2, out1_A,out2_A,out1_B,out2_B,nTimes);

    //SAVE 1: save round 1 output in a file
    save_round1_output(out1_A,  out1_B, out2_A, out2_B);

    //SAVE 2: save round 2 output in a file
    save_round2_output(SC_out1, SC_out2);

    //round1- fetch output from a file
    //fetch_round1_output(out1_A,  out1_B, out2_A, out2_B);

    //round2- fetch output from a file
    //fetch_round2_output(SC_out1, SC_out2);*/

    for(int i = 0; i< nRuns; i++)//runs the rounds for nRuns times
    {
        //round1
        round1(K1,x1,K2,x2,out1_A,out2_A,out1_B,out2_B,nTimes);
        //fetch_round1_output(out1_A,  out1_B, out2_A, out2_B);

        //round2
        round2(SC_out1, SC_out2, out1_A,out2_A,out1_B,out2_B,nTimes);
        //fetch_round2_output(SC_out1, SC_out2);

#ifndef TEST_PRF_LOOKUP     //party 1 phase 3 without lookup implementation

        //===============party 1 phase 3 without lookup table==================
        start_p31 = chrono::system_clock::now();
        out1Z3.matByVec(Rmat, SC_out1); // compute matrix-by-vector multiply
        timer_phase31 += (std::chrono::system_clock::now() - start_p31).count();

        //===============party 2 phase 3 without lookup table==================
        start_p32 = chrono::system_clock::now();
        out2Z3.matByVec(Rmat, SC_out2);    //compute matrix-by-vector multiply
        timer_phase32 += (std::chrono::system_clock::now() - start_p32).count();
#endif

#ifdef TEST_PRF_LOOKUP  //party 1 phase 3 with lookup implementation
        //round3/usedLookupTable
        start_round3_p1 = std::chrono::system_clock::now();
        round3_party1_lookup(out1Z3, SC_out1,lookup_prf);
        timer_phase31 += (chrono::system_clock::now() - start_round3_p1).count();

        start_round3_p2 = std::chrono::system_clock::now();
        round3_party2_lookup(out2Z3, SC_out2,lookup_prf);
        timer_phase32 += (chrono::system_clock::now() - start_round3_p2).count();
#endif
        start_final_output = std::chrono::system_clock::now();
        final_protocol_output(out_protocol_result,out1Z3, out2Z3);
        timer_final_output += (chrono::system_clock::now() - start_final_output).count();
    }

}

/*
 * PRF_DM includes pre-processing
 */

void PRF_DM_wpreproc(unsigned int nTimes,  int nRuns, int nStages) {

    randomWord(1); // use seed=1
    vector<uint64_t> K1(toeplitzWords), K2(toeplitzWords);  //Key to the protocol
    PackedZ2<N_COLS> x1, x2;            //input of the protocol
    std::vector<PackedZ3<81> > Rmat(256); // generate a 81x256 matrix

    //Declaring output variables for wPRF protocol and centralized wPRF
    PackedZ3<81> outZ3;         //output of packed_centralized implementation used for testing
    PackedZ3<81> out1Z3, out2Z3;                     // party i's protocol output share
    PackedZ3<81> outSum;            //final output of entire dark matter wPRF
    outSum.reset(); //reset the output

    //preprocessing
    DM_preprocess(nTimes);

    //set input
    set_input(K1, x1, K2, x2, Rmat);

    std::cout<<" Number of Runs: "<<nRuns<<std::endl;

    PRF_DM(K1, x1, K2, x2, Rmat, out1Z3, out2Z3, nRuns, nTimes); // Calling the main function

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
