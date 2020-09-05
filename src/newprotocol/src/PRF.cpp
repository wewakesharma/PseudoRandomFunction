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
#include <typeinfo>//to determine the type of variables
#include "Timing.hpp"
#include <chrono>

// in Toeplitz-by-x.hpp
// #define N_ROWS 256
// #define N_COLS 256

using namespace std;

long timerPRF = 0;
long timer_phase3 = 0;

// A place to store the results from pre-processing
static std::vector< std::vector<uint64_t> > rAs;
static std::vector< PackedZ2<N_ROWS> > rbs, rzs;
static std::vector< PackedZ2<N_COLS> > rxs;



//The file has been now shifted to packed_PRF_central

void PRF_packed_naive_test(std::vector<uint64_t>& K1, PackedZ2<N_COLS>& x1, std::vector<uint64_t>& K2,
                          PackedZ2<N_COLS>& x2, std::vector< PackedZ3<81> >& Rmat, PackedZ3<81>& out1Z3,
                          PackedZ3<81>& out2Z3, int i)
{
    cout<<endl<<"We are in packed unit test of PRF"<<endl;
    //1.perform X = x1+ x2 (on vectors)
    PackedZ2<N_COLS> X = x1; //declare a variable
    //X.add(x1); x = x1
    X.add(x2);  //x = x1 + x2


    //2.perform K = k1 + k2 (on matrix)

    std::vector<uint64_t> K(toeplitzWords);
    for (int i = 0; i < K1.size(); i++)
    {
        K[i] = K1[i] ^ K2[i];
    }
    PackedZ2<N_COLS> outKX;
    outKX.toeplitzByVec(K,X);

    PackedZ3<256> outKX_Z3;//packed Z3

    std::vector<unsigned int> outKX_unsgn;//unsigned int of outKX i.e.(K*x)
    outKX.toArray(outKX_unsgn);

    PackedZ3<81> outZ3;//final Z3 output
    outKX_Z3.fromArray(outKX_unsgn);//converting unsigned int to PackedZ2

    outZ3.matByVec(Rmat,outKX_Z3);//output of randmat*K*x

    PackedZ3<81>out_12_Z3 = out1Z3;
    out_12_Z3.add(out2Z3);//merged output from parameters

    if(out_12_Z3 == outZ3)
        cout<<"Test passed";
    else
        cout<<"Test fails";
}



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

        chrono::time_point<std::chrono::system_clock> start = chrono::system_clock::now();

        // Party1 computes locally K1 times x1, and adds to out1_A,out2_A
        out1_A.add(out2_A);           // out1 ^= out2
        out2_A.toeplitzByVec(K1, x1); // K1 times x1
        out1_A.add(out2_A);           // sum of all terms

        timerAxpBP1 += (chrono::system_clock::now() - start).count();


        start = chrono::system_clock::now();

        // Party2 computes locally K2 times x2, and adds to out1_B,out2_N
        out1_B.add(out2_B);           // out1 ^= out2
        out2_B.toeplitzByVec(K2, x2); // K2 times x2
        out1_B.add(out2_B);           // sum of all terms

        timerAxpBP2 += (chrono::system_clock::now() - start).count();

        //end of phase 1

        PackedZ2<N_SIZE>& y1 = out1_A;
        PackedZ2<N_SIZE>& y2 = out1_B;
        PackedZ3<N_SIZE> out1, out2;

        SC_Party2_1(y2, i);
        SC_Party1(y1, out1, i);
        SC_Party2_2(y2, out2, i);

        //end of phase 2

    start = chrono::system_clock::now();

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

    PackedZ3<81> out1Z3;                     // 81-vector
    PackedZ3<81> out2Z3;                     // 81-vector

    auto start = std::chrono::system_clock::now();

    //TODO: write phase 1 function
    for (int i = 0; i < nRuns; i++) {

        PRF(K1, x1, K2, x2, Rmat, out1Z3, out2Z3, i); // R = randomization matrix
        //PRF_packed_naive_test(K1, x1, K2, x2, Rmat, out1Z3, out2Z3, i);
        //PRF_unpacked_test() /* just a placeholder*/
    }

    timerPRF += (chrono::system_clock::now() - start).count();



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

