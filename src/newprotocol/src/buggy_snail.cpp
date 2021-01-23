//
// Created by Vivek Sharma on 1/22/21.
//

#include <iostream>

#include "packedMod2.hpp"
#include "Toeplitz-by-x.hpp"
#include "OT.hpp"
#include "mains.hpp"
#include "Timing.hpp"
#include <chrono>
#include "packedMod3.hpp"
#include "utils.hpp"
#include "lookup_functions.h"
#include "PRF.hpp"
#include "buggy_snail.h"

using namespace std;

/*
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
 */


/*
void display_timing() {
    using Clock = std::chrono::system_clock;
    using Duration = Clock::duration;
    //std::cout << Duration::period::num << " , " << Duration::period::den << '\n';
    float time_unit_multiplier = 1;
    if (Duration::period::den == 1000000000)
        time_unit_multiplier = 0.001; //make nanosecond to microsecond
    else if (Duration::period::den == 1000000)
        time_unit_multiplier = 1;   //keep the unit as microsecond

    cout << endl << "AX + B execution time " << endl;
    cout << "Party 1: " << (timerAxpBP1 * time_unit_multiplier) << " microseconds" << endl;
    cout << "Party 2: " << (timerAxpBP2 * time_unit_multiplier) << " microseconds" << endl;
    std::cout << "Time to execute phase 1 (K*X): " <<
              ((timerAxpBP1 + timerAxpBP2) * time_unit_multiplier) << " microseconds" << std::endl;
    std::cout << "Number of rounds per second for phase 1: "
              << (1000 / ((timerAxpBP1 + timerAxpBP2) * time_unit_multiplier) * 1000000) << std::endl;
    std::cout << "=====================================================" << std::endl;
    std::cout << std::endl << "Share Conversion execution time " << std::endl;
    std::cout << "Party 1: " << (timerSCP1 * time_unit_multiplier) << " microseconds" << std::endl;
    std::cout << "Party 2: " << (timerSCP2 * time_unit_multiplier) << " microseconds" << std::endl;
    std::cout << "Time to execute phase 2(Share Conversion): " <<
              ((timerSCP1 + timerSCP2) * time_unit_multiplier) << " microseconds" << std::endl;
    std::cout << "Number of rounds per second for phase 2: "
              << (1000 / ((timerSCP1 + timerSCP2) * time_unit_multiplier) * 1000000) << std::endl;
    std::cout << "=====================================================" << std::endl;
#ifndef TEST_PRF_LOOKUP
    timer_phase3 = std::max(timer_phase31,
                            timer_phase32);       //taking maximum of both phases to emulate simultaneous execution
    std::cout << "\nphase 3 party 1: " << (timer_phase31 * time_unit_multiplier) << " microseconds" << std::endl;
    std::cout << "\nphase 3 party 2: " << (timer_phase32 * time_unit_multiplier) << " microseconds" << std::endl;
    std::cout << "\nTime to execute phase 3: " <<
              (timer_phase3 * time_unit_multiplier) << " microseconds" << std::endl;
    std::cout << "Number of rounds per second for phase 3: " << (1000 / (timer_phase3 * time_unit_multiplier) * 1000000)
              << std::endl;
    timerPRF = (timerAxpBP1 + timerAxpBP2) + (timerSCP1 + timerSCP2) + timer_phase3;
#endif
}*/
    /*=======================================ARCHIVE SECTION=============================
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

/*
 * Main function that drives PRF buggy snail program
 * The function is as same as PRF_DM_wpreproc but it will be used for debugging
 */
void DM_preprocess(int nTimes)
{
    initGlobals();      //initialize some global variables
    preProc_Toeplitz_by_x(nTimes * 2); // pre-processing for two runs
    preProc_OT(nTimes); //preprocess for OT, generate ra, rn, rx, and z
}

void set_snail_input(std::vector<uint64_t>& K1,PackedZ2<N_COLS>& x1,
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

    start_p1 = chrono::system_clock::now();
    party1_local_computation(K1, x1, out1_A,out2_A);
    timerAxpBP1 += (chrono::system_clock::now() - start_p1).count();

    start_p2 = chrono::system_clock::now();
    party2_local_computation(K2, x2, out1_B,out2_B);
    timerAxpBP2 += (chrono::system_clock::now() - start_p2).count();
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
void round3_party1(PackedZ3<81>& out1Z3, PackedZ3<N_SIZE>& SC_out1,std::vector<std::vector<PackedZ3<81> > >& lookup_prf)
{
    usedLookupTable(out1Z3,SC_out1,lookup_prf);
}

void round3_party2(PackedZ3<81>& out2Z3, PackedZ3<N_SIZE>& SC_out2,std::vector<std::vector<PackedZ3<81> > >& lookup_prf)
{
    usedLookupTable(out2Z3,SC_out2,lookup_prf);
}

void final_protocol_output(PackedZ3<81>& out_protocol_result, PackedZ3<81>& out1Z3, PackedZ3<81>& out2Z3)
{
    out_protocol_result = out1Z3;
    out_protocol_result += out2Z3;
}

void DM_snail(int nRuns, int nTimes)
{

    randomWord(1); // use seed=1
    vector<uint64_t> K1(toeplitzWords), K2(toeplitzWords);  //Key to the protocol
    PackedZ2<N_COLS> x1, x2;            //input of the protocol
    std::vector<PackedZ3<81> > Rmat(256); // generate a 81x256 matrix
    std::vector<std::vector<PackedZ3<81> > > lookup_prf(16); //lookup table

    PackedZ3<N_SIZE> SC_out1, SC_out2;        //phase 2 output

    PackedZ3<81> out1Z3;                     // party 1's protocol output
    PackedZ3<81> out2Z3;                     // party 2's protocol output

    PackedZ3<81> out_protocol_result;            //final output of entire dark matter wPRF
    out_protocol_result.reset();

    PackedZ2<N_ROWS> out1_A, out2_A, out1_B, out2_B;//variables required for phase 2 computation

    //preprocessing
    DM_preprocess(nTimes);//performs the preprocessing and set the variables defined in PRF.hpp

    //set input
    set_snail_input(K1,x1,K2,x2,Rmat);//set the randomized inputs for key and input

    //setup lookup preprocessing
    setup_table(lookup_prf, Rmat);

    //round1
    round1(K1,x1,K2,x2,out1_A,out2_A,out1_B,out2_B,nTimes);

    //round2
    round2(SC_out1, SC_out2, out1_A,out2_A,out1_B,out2_B,nTimes);

    //round3/usedLookupTable
    round3_party1(out1Z3, SC_out1,lookup_prf);

    round3_party2(out2Z3, SC_out2,lookup_prf);

    final_protocol_output(out_protocol_result,out1Z3, out2Z3);
    std::cout<<out_protocol_result<<std::endl;

    PackedZ3<81> central_outZ3;
    PRF_packed_centralized(K1,x1,K2,x2,Rmat,central_outZ3,1);   //centralized implementation for testing
    cout << "Output of PRF centralized implementation, outZ3=" << central_outZ3 << endl;

    if(out_protocol_result == central_outZ3) //outZ3 is the output of centralized and outSum is output of DM wPRF protocol.
        std::cout<<std::endl<<"Test passed"<<std::endl;
    else
        std::cout<<std::endl<<"Test failed"<<std::endl;
    //display_timing();       //displays the timing and number of rounds in each phase of the protocol
}
