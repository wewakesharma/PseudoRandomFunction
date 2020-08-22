// Copyright (C) 2020 Tzipora Halevi, MIT license - see LICENSE file
/** Toeplitz-by-x.cpp
 *  - implementing a protocol for A*x (aka A*x+b) mod 2
 *    A is a Toeplitz matrix
 */
#include <cassert>
#include "packedMod2.hpp"
#include "Toeplitz-by-x.hpp"
#include "OT.hpp"
#include "mains.hpp"
#include "Timing.hpp"
#include <typeinfo>//to determine the type of variables

// in Toeplitz-by-x.hpp
// #define N_ROWS 256
// #define N_COLS 256

using namespace std;

// A place to store the results from pre-processing
static std::vector< std::vector<uint64_t> > rAs;
static std::vector< PackedZ2<N_ROWS> > rbs, rzs;
static std::vector< PackedZ2<N_COLS> > rxs;

long timerPRF = 0;
long timer_phase3 = 0;

void PRF(vector<uint64_t>& K1, PackedZ2<N_COLS>& x1, vector<uint64_t>& K2,
        PackedZ2<N_COLS>& x2, std::vector< PackedZ3<81> >& Rmat, PackedZ3<81>& out1Z3,
        PackedZ3<81>& out2Z3, int i)
{
        PackedZ2<N_ROWS> out1_A, out2_A, out1_B, out2_B;

        topelitz_Party2_1(x2, 2*i);
        topelitz_Party1(out1_A, K1, 2*i);
        topelitz_Party2_2(out1_B, x2, 2*i);

        // Second run, a protocol for K2 times x1
        topelitz_Party2_1(x1, 2*i+1);
        topelitz_Party1(out2_B, K2, 2*i+1);
        topelitz_Party2_2(out2_A, x1, 2*i+1);

        // Party1 computes locally K1 times x1, and adds to out1_A,out2_A
        out1_A.add(out2_A);           // out1 ^= out2
        out2_A.toeplitzByVec(K1, x1); // K1 times x1
        out1_A.add(out2_A);           // sum of all terms

        // Party2 computes locally K2 times x2, and adds to out1_B,out2_N
        out1_B.add(out2_B);           // out1 ^= out2
        out2_B.toeplitzByVec(K2, x2); // K2 times x2
        out1_B.add(out2_B);           // sum of all terms

        PackedZ2<N_SIZE>& y1 = out1_A;
        PackedZ2<N_SIZE>& y2 = out1_B;
        PackedZ3<N_SIZE> out1, out2;

        SC_Party2_1(y2, i);
        SC_Party1(y1, out1, i);
        SC_Party2_2(y2, out2, i);

    chrono::time_point<std::chrono::system_clock, std::chrono::microseconds> start = chrono::system_clock::now();
    out1Z3.matByVec(Rmat, out1); // compute matrix-by-vector multiply
    out2Z3.matByVec(Rmat, out2); // compute matrix-by-vector multiply

    timer_phase3 += (std::chrono::system_clock::now() - start).count();

}


void PRF_DM(unsigned int nTimes,  int nRuns, int nStages) {

    randomWord(1); // use seed=1
    vector<uint64_t> K1(toeplitzWords), K2(toeplitzWords);
    PackedZ2<N_COLS> x1, x2;

    PackedZ2<N_ROWS> out1_A, out2_A, out1_B, out2_B;

    initGlobals();  // initialize some global variables

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

    x1.randomize();
    x2.randomize();

    //chrono::time_point<std::chrono::system_clock, std::chrono::seconds> start = chrono::system_clock::now();
    auto start = std::chrono::system_clock::now();
    auto start_seconds = std::chrono::time_point_cast<std::chrono::microseconds>(start);

    PackedZ3<81> out1Z3;                     // 81-vector
    PackedZ3<81> out2Z3;                     // 81-vector

    //TODO: write phase 1 function
    for (int i = 0; i < nRuns; i++) {

        PRF(K1, x1, K2, x2, Rmat, out1Z3, out2Z3, i); // R = randomization matrix
        timerPRF += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start_seconds).count();
        //PRF_packed_unit_test(K1, x1, K2, x2, Rmat, out1Z3, out2Z3, i); //for packed unit testing of PRF code
        //PRF_unpacked_test() /* just a placeholder*/
    }
    



    /*
        for (int i = 0; i < nRuns; i++) {
        // First run, a protocol for K1 times x2
        topelitz_Party2_1(x2, 2*i);
        topelitz_Party1(out1_A, K1, 2*i);
        topelitz_Party2_2(out1_B, x2, 2*i);

        // Second run, a protocol for K2 times x1
        topelitz_Party2_1(x1, 2*i+1);
        topelitz_Party1(out2_B, K2, 2*i+1);
        topelitz_Party2_2(out2_A, x1, 2*i+1);

        // Party1 computes locally K1 times x1, and adds to out1_A,out2_A
        out1_A.add(out2_A);           // out1 ^= out2
        out2_A.toeplitzByVec(K1, x1); // K1 times x1
        out1_A.add(out2_A);           // sum of all terms

        // Party2 computes locally K2 times x2, and adds to out1_B,out2_N
        out1_B.add(out2_B);           // out1 ^= out2
        out2_B.toeplitzByVec(K2, x2); // K2 times x2
        out1_B.add(out2_B);           // sum of all terms
    } //run nRuns = 1M times

    chrono::duration<double> elapsed_seconds_part1 = chrono::system_clock::now() - start;

    cout << "number of runs= " << nRuns <<  ", elapsed time for AX+B phase:  " << elapsed_seconds_part1.count() << endl ;

    if (nStages == 1)
        return; //we're done here
    //at this point we have the outputs of each party

        //============================OT evaluation==========================

    PackedZ2<N_SIZE>& y1 = out1_A;
    PackedZ2<N_SIZE>& y2 = out1_B;
    PackedZ3<N_SIZE> out1, out2;

    chrono::time_point<std::chrono::system_clock> start2 = chrono::system_clock::now();


    for (int i = 0; i < nRuns; i++) {
        SC_Party2_1(y2, i);
        SC_Party1(y1, out1, i);
        SC_Party2_2(y2, out2, i);
        }

    //test the results

    chrono::duration<double> elapsed_seconds_SC = chrono::system_clock::now() - start2;

    cout << endl << "elapsed time for SC  phase:  " << elapsed_seconds_SC.count() << "  s\n";

    if (nStages == 2)
        return; //we're done here

    //phase 3 implementation
  //Here we start the next phase were we deal with Z3 numbers
    // fill with random Z3 elements

    chrono::time_point<std::chrono::system_clock> start3 = chrono::system_clock::now();

    for (int i = 0; i < nRuns; i++) {
        out1Z3.matByVec(Rmat, out1); // compute matrix-by-vector multiply
        out2Z3.matByVec(Rmat, out2); // compute matrix-by-vector multiply
    }

    chrono::duration<double> elapsed_seconds_P3 = chrono::system_clock::now() - start3;

    cout << endl << "elapsed time for Phase3  phase:  " << elapsed_seconds_P3.count() << "  s\n";
*/
}

void display_Phase3_runtime()
{
    std::cout<<std::endl<<"Phase 3(Randomization) execution time "<< timer_phase3 <<std::endl;
}

void display_PRF_runtime()
{
    std::cout<<std::endl<<"PRF execution time "<< timerPRF <<std::endl;
}

