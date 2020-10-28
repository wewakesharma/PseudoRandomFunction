//
// Created by Vivek Sharma on 8/24/20.
//
#include <cassert>
#include "packedMod2.hpp"
#include "Toeplitz-by-x.hpp"
#include "OT.hpp"
#include "mains.hpp"
#include <typeinfo>//to determine the type of variables
#include "Timing.hpp"

// in Toeplitz-by-x.hpp
// #define N_ROWS 256
// #define N_COLS 256

using namespace std;

long timer_PRF_packed = 0;
long timer_packed_cent_p1 = 0;
long timer_packed_cent_p2 = 0;
long timer_packed_cent_p3 = 0;
long timer_packed_cent_p2 = 0;

// A place to store the results from pre-processing
static std::vector< std::vector<uint64_t> > rAs;
static std::vector< PackedZ2<N_ROWS> > rbs, rzs;
static std::vector< PackedZ2<N_COLS> > rxs;

void PRF_packed_central(std::vector<uint64_t>& K1, PackedZ2<N_COLS>& x1, std::vector<uint64_t>& K2,
                          PackedZ2<N_COLS>& x2, std::vector< PackedZ3<81> >& Rmat, PackedZ3<81>& out1Z3,
                          PackedZ3<81>& out2Z3, int i)
{
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

    auto start_p1 = chrono::system_clock::now();
    outKX.toeplitzByVec(K,X);
    timer_packed_cent_p1 += (std::chrono::system_clock::now() - start_p1).count();

    PackedZ3<256> outKX_Z3;//packed Z3

 //   std::vector<unsigned int> outKX_unsgn;//unsigned int of outKX i.e.(K*x)
 //   outKX.toArray(outKX_unsgn);

    PackedZ2<N_SIZE> hi; //will be initialized to 0
    auto start_p2 = chrono::system_clock::now();

    auto start_p2 = chrono::system_clock::now();
    outKX_Z3.makeFromBits(hi.bits, outKX.bits);
    timer_packed_cent_p2 += (std::chrono::system_clock::now() - start_p1).count();
 //   outKX_Z3.fromArray(outKX_unsgn);//converting unsigned int to PackedZ2
    timer_packed_cent_p2 += (std::chrono::system_clock::now() - start_p2).count();

    PackedZ3<81> outZ3;//final Z3 output
    auto start_p3 = chrono::system_clock::now();
    outZ3.matByVec(Rmat,outKX_Z3);//output of randmat*K*x
    timer_packed_cent_p3 += (std::chrono::system_clock::now() - start_p3).count();

    PackedZ3<81>out_12_Z3 = out1Z3;
    out_12_Z3.add(out2Z3);//merged output from parameters

}

void PRF_packed(int nTimes,  int nRuns, int nStages)
{
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
        PRF_packed_central(K1, x1, K2, x2, Rmat, out1Z3, out2Z3, i);
    }
}

<<<<<<< HEAD

void display_times(int nRuns)
{
    std::cout<<"Time in ms for p1, "<<nRuns << " runs = " << timer_packed_cent_p1<<  std::endl;
    std::cout<<"Time in ms for p2, "<<nRuns << " runs = " << timer_packed_cent_p2<<  std::endl;
    std::cout<<"Time in ms for p3,  "<<nRuns << " runs = " <<timer_packed_cent_p3 << std::endl;
=======
void display_time_packed(int nRuns)
{
    std::cout<<std::endl<<"Time to execute phase 1 in ms for "<<nRuns << " runs = " << timer_packed_cent_p1<<  std::endl;
    std::cout<<"Time to execute phase 2 in ms for "<<nRuns << " runs = " <<timer_packed_cent_p2 << std::endl;
    std::cout<<"Time to execute phase 3 in ms for "<<nRuns << " runs = " <<timer_packed_cent_p3 << std::endl;
>>>>>>> ca938fa7c904993d3b3f3f120e57e16ac3484718
}