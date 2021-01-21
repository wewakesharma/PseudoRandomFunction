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
#include <typeinfo>//to determine the type of variables
#include "Timing.hpp"
#include <chrono>
#include "packedMod2.hpp"
#include "packedMod3.hpp"
#include "utils.hpp"
#include "lookup_functions.h"

// in Toeplitz-by-x.hpp
// #define N_ROWS 256
// #define N_COLS 256

using namespace std;


// Declaring the global variables for lookup implementation
std::vector<std::vector<PackedZ3<81> > > Rmat16_prf(16); //Rmat16_prf vector of size 16 X 16 X 81 (GLOBAL)
std::vector<std::vector<PackedZ3<81> > > lookup_prf(16); //lookup table

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

long timerreformat = 0;
long timerlookup = 0;
long timersubtract = 0;

#ifdef LOOKUP_TIME
long timer_create_lookup = 0;
long timer_use_lookup = 0;
#endif

// A place to store the results from pre-processing
static std::vector< std::vector<uint64_t> > rAs;
static std::vector< PackedZ2<N_ROWS> > rbs, rzs;
static std::vector< PackedZ2<N_COLS> > rxs;
PackedZ3<81> out_dummy;

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

#ifdef LOOKUP
void independent_lookup_implementation(int nRuns)
{

        //generate Rmat with 16 matrix of size 81 X 16.
        std::vector<std::vector<PackedZ3<81> > > Rmat16_prf(16);

        for(int matrix_count = 0; matrix_count < 16; matrix_count++)//go over 16 matrix
        {
            Rmat16_prf[matrix_count].resize(16);        //resize each matrix to have 16 columns
            for (auto &col : Rmat16_prf[matrix_count]) // iterate over the columns
                col.randomize();                    //call the randomize function in packedMod3.hpp
        }


#ifdef LOOKUP_DEBUG
        //printing the values of Rmat16_prf
    std::cout<<"The number of matrices is "<<Rmat16_prf.size()<<std::endl;
    for(int i = 0; i< 16;i++)
    {
        std::cout<<Rmat16_prf[i]<<std::endl;
    }
#endif

#ifdef LOOKUP
        //generate a lookup table with all the possibilities of multiplication of Rmat with 2^16 inputs.
        std::vector<std::vector<PackedZ3<81> > > lookup_prf(16); //table has 16 rows and 65536 columns and each element is PackedZ3

#ifdef LOOKUP_TIME
        std::chrono::time_point<std::chrono::system_clock> start_create_lookup = std::chrono::system_clock::now();
#endif
        create_lookup_prf(Rmat16_prf,lookup_prf);

#ifdef LOOKUP_TIME
        timer_create_lookup += (std::chrono::system_clock::now() - start_create_lookup).count();
#endif

#ifdef LOOKUP_DEBUG
        std::cout<<"The lookup table has been generated successfully"<<std::endl;
#endif

    PackedZ3<81> totalTime;

    for(int iteration_count = 0; iteration_count < nRuns; iteration_count++)//running use of lookup table nRuns times
    {
        uint64_t random_input16[16]; //16 random words of size 16 bits
        uint64_t random_input64[4]; //4 random words of size 64 bits

        for(int word_count  = 0; word_count < 4; word_count++)
        {
            random_input64[word_count] = 0;
            random_input64[word_count] = randomWord(0); //randomWord generates 64 bits of input in Z2.
            random_input16[4*word_count+0] = ((random_input64[word_count]) & 65535);
            random_input16[4*word_count+1] = ((random_input64[word_count]>>16) & 65535);
            random_input16[4*word_count+2] = ((random_input64[word_count]>>32) & 65535);
            random_input16[4*word_count+3] = ((random_input64[word_count]>>48) & 65535);
            //std::cout<<random_input64[i]<<std::endl;
        }

#ifdef LOOKUP_DEBUG
        std::cout<<"The input has been generated successfully, they are: " <<std::endl;
#endif


        std::vector<PackedZ3<81> > result_table(16); //stores the result of multiplication using lookup table
        for(int i =0; i< 16; i++)
        {
            result_table[i].reset();
        }


#ifdef LOOKUP_TIME
        std::chrono::time_point<std::chrono::system_clock> start_use_lookup = std::chrono::system_clock::now();
#endif
        //use_lookup_prf(lookup_prf, random_input16,result_table);
        for(int word_count = 0; word_count < 16; word_count++)
        {
            uint64_t value = random_input16[word_count];
            result_table[word_count] = lookup_prf[word_count][value];
        }

        //add up the 16 count of packedmod3 values
        PackedZ3<81> result_sum;
        result_sum = result_table[0];
        for(int count = 1; count < 16; count++)
        {
            result_sum += result_table[count];
        }

#ifdef LOOKUP_TIME
        timer_use_lookup += (std::chrono::system_clock::now() - start_use_lookup).count();

        totalTime+= result_sum;

#endif

#ifdef LOOKUP_DEBUG
        std::cout<<"Printing the output value "<<std::endl;
    std::cout<<"The final output is "<<result_sum<<std::endl;
#endif

#endif
    }
    cout << "in independent_lookup_implementation, " << "total Time=" << totalTime << endl;
    cout << "in independent_lookup_implementation, " << "nruns-" << nRuns << endl;
}
#endif

#ifdef LOOKUP_TIME
void exec_lookup_timing()
{
    std::cout<<"Time to create a lookup table(just once) "<<timer_create_lookup<<std::endl;
    std::cout<<"Time elapsed in using lookup table "<<timer_use_lookup<<std::endl;
}
#endif


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
    timer_phase3 = std::max(timer_phase31,timer_phase32);       //taking maximum of both phases to emulate simultaneous execution
    std::cout<<"\nphase 3 party 1: "<<(timer_phase31 * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"\nphase 3 reformat: "<<(timerreformat * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"\nphase 3 uselookup: "<<(timerlookup * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"\nphase 3 subtract: "<<(timersubtract * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"\nphase 3 party 2: "<<(timer_phase32 * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"\nTime to execute phase 3: "<<
             (timer_phase3 * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"Number of rounds per second for phase 3: "<<(1000/(timer_phase3*time_unit_multiplier)*1000000)<<std::endl;
    std::cout<<"====================================================="<<std::endl;
    timerPRF = (timerAxpBP1 + timerAxpBP2) + (timerSCP1 + timerSCP2) + timer_phase3;
    std::cout<<"\nTime to execute entire PRF protocol: "<<
             (timerPRF * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"Number of rounds per second for entire PRF: "<<(1000/(timerPRF*time_unit_multiplier)*1000000)<<std::endl;

/*
    timer_round1 = std::max(timer_round1_p1, timer_round1_p2) + timer_round1_mask;
    timer_round2 = std::max(timer_round2_p1, timer_round2_p2) + timer_round2_mask;
    timer_round3 = std::max(timer_round3_p1, timer_round3_p2);
    timer_np_prf = timer_round1 + timer_round2 + timer_round3;*/
}

void PRF_DM(vector<uint64_t>& K1, PackedZ2<N_COLS>& x1, vector<uint64_t>& K2,
        PackedZ2<N_COLS>& x2, std::vector< PackedZ3<81> >& Rmat, PackedZ3<81>& out1Z3,
        PackedZ3<81>& out2Z3, int i)
{

    PackedZ3<81> result_out1_lsb, result_out1_msb;
    PackedZ3<81> result_out2_lsb, result_out2_msb;

    std::vector<uint64_t> out1_lsb, out1_msb;
    std::vector<uint64_t> out2_lsb, out2_msb;

        PackedZ2<N_ROWS> out1_A, out2_A, out1_B, out2_B;
        result_out1_lsb.reset();
        result_out1_msb.reset();
        result_out2_lsb.reset();
        result_out2_msb.reset();

    //NOTE: The timing for Toeplitz_partyx[_x] is within the function. Please do not panic.

    //Start of phase 1:common for both without and with lookup

    // First run, a protocol for K1 times x2
        topelitz_Party2_1(x2, 2*i);
        topelitz_Party1(out1_A, K1, 2*i);
        topelitz_Party2_2(out1_B, x2, 2*i);

        // Second run, a protocol for K2 times x1
        topelitz_Party2_1(x1, 2*i+1);
        topelitz_Party1(out2_B, K2, 2*i+1);
        topelitz_Party2_2(out2_A, x1, 2*i+1);

        chrono::time_point<std::chrono::system_clock> start_p1 = chrono::system_clock::now();

        // Party1 computes locally K1 times x1, and adds to out1_A,out2_A
        out1_A.add(out2_A);           // out1 ^= out2
        out2_A.toeplitzByVec(K1, x1); // K1 times x1
        out1_A.add(out2_A);           // sum of all terms

        timerAxpBP1 += (chrono::system_clock::now() - start_p1).count();


        auto start_p2 = chrono::system_clock::now();

        // Party2 computes locally K2 times x2, and adds to out1_B,out2_N
        out1_B.add(out2_B);           // out1 ^= out2
        out2_B.toeplitzByVec(K2, x2); // K2 times x2
        out1_B.add(out2_B);           // sum of all terms

        timerAxpBP2 += (chrono::system_clock::now() - start_p2).count();
        //=============end of phase 1==================

        //============Start of Phase 2(common for both lookup and without lookup===============
        PackedZ2<N_SIZE>& y1 = out1_A;
        PackedZ2<N_SIZE>& y2 = out1_B;
        PackedZ3<N_SIZE> out1, out2;

        SC_Party2_1(y2, i);             //The implementation details for SC functions are within the function.
        SC_Party1(y1, out1, i);
        SC_Party2_2(y2, out2, i);
        //=============end of phase 2=================

    auto start_p31 = chrono::system_clock::now();

    #ifndef TEST_PRF_LOOKUP     //party 1 phase 3 without lookup implementation
        out1Z3.matByVec(Rmat, out1); // compute matrix-by-vector multiply
    #endif

    #ifdef TEST_PRF_LOOKUP  //party 1 phase 3 with lookup implementation

        //step3a. resize the output vector
        out1_lsb.resize(16);
        out1_msb.resize(16);

    auto start_reformat = chrono::system_clock::now();
        //step3b. reformat the input
        reformat_input(out1_lsb,out1.lsbs());
        reformat_input(out1_msb,out1.msbs());
    timerreformat += (chrono::system_clock::now() - start_reformat).count();

    auto start_uselookup = chrono::system_clock::now();
        //step3c. use lookup
        uselookup(result_out1_lsb,out1_lsb,lookup_prf);
        uselookup(result_out1_msb,out1_msb,lookup_prf); //use of lookup table
    timerlookup += (chrono::system_clock::now() - start_uselookup).count();

    auto start_subtract = chrono::system_clock::now();
        //step3d. subtract to get the final output
        out1Z3 = result_out1_lsb;
        out1Z3.subtract(result_out1_msb);
    timersubtract += (chrono::system_clock::now() - start_subtract).count();
    #endif

    timer_phase31 += (std::chrono::system_clock::now() - start_p31).count();

    auto start_p32 = chrono::system_clock::now();

    #ifndef TEST_PRF_LOOKUP         //party 2 phase 3 without lookup implementation
    out2Z3.matByVec(Rmat, out2);    //compute matrix-by-vector multiply
    #endif

    #ifdef TEST_PRF_LOOKUP  //party 2 phase 3 lookup implementation
    //step3a. resize the output vector
    out2_lsb.resize(16);    //resize the input space to accomodate 16 bits
    out2_msb.resize(16);

    //step3b. reformat the input
    reformat_input(out2_lsb,out2.lsbs());
    reformat_input(out2_msb,out2.msbs());   //reformat input into 16 words of 16 bits each

    //step3c. use lookup
    uselookup(result_out2_lsb,out2_lsb,lookup_prf); //use lookup function
    uselookup(result_out2_msb,out2_msb,lookup_prf);

    //step3d. subtract to get the final output
    out2Z3 = result_out2_lsb;
    out2Z3.subtract(result_out2_msb);
    #endif

    timer_phase32 += (std::chrono::system_clock::now() - start_p32).count();

    //timerPRF += (chrono::system_clock::now() - start_prf).count();

    out_dummy += out1Z3;
    out_dummy += out2Z3;        //creating and assigning output to dummy variable so that the timing don't get skipped
}

/*
 * PRF_DM includes pre-processing
 */

void PRF_DM_wpreproc(unsigned int nTimes,  int nRuns, int nStages) {

    randomWord(1); // use seed=1
    vector<uint64_t> K1(toeplitzWords), K2(toeplitzWords);
    PackedZ2<N_COLS> x1, x2;
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


    //Creation of Lookup table
#ifdef TEST_PRF_LOOKUP                  //reformat Rmat and create a lookup table only if LOOKUP implementation is required
    reformat_Rmat(Rmat16_prf, Rmat);        //convert (81 X 256) matrix to (16 x 16 X 81) matrix
    create_lookup_table(Rmat16_prf, lookup_prf);        //(a lookup table of 16 X 2^16) is generated
    std::cout<<"Lookup table created"<<std::endl;
#endif

    //TODO: write phase 1 function
    for (int i = 0; i < nRuns; i++) {   //nRuns is the number of runs, for timing we keep it at 1000

        PRF_DM(K1, x1, K2, x2, Rmat, out1Z3, out2Z3, i); // R = randomization matrix
    //    PRF_packed_centralized_test(K1, x1, K2, x2, Rmat, out1Z3, out2Z3, i);
        outSum = out1Z3;
        outSum += out2Z3;   //computing the output of entire PRF by adding the shares of output by both the parties
    }
    cout << "Output of PRF Dark matter, outSum=" << outSum << endl;
    PRF_packed_centralized(K1,x1,K2,x2,Rmat,outZ3,1);   //centralized implementation for testing
    cout << "Output of PRF centralized implementation, outZ3=" << outZ3 << endl;

    if(outSum == outZ3) //outZ3 is the output of centralized and outSum is output of DM wPRF protocol.
        std::cout<<std::endl<<"Test passed"<<std::endl;
    else
        std::cout<<std::endl<<"Test failed"<<std::endl;
    display_timing();
}

