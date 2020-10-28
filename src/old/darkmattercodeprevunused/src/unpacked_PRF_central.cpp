//
// Created by Vivek Sharma on 8/25/20.
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

long timer_PRF_unpacked = 0;
long timer_unpacked_cent_p1 = 0;
long timer_unpacked_cent_p2 = 0;
long timer_unpacked_cent_p3 = 0;

// A place to store the results from pre-processing
static std::vector< std::vector<uint64_t> > rAs;
static std::vector< PackedZ2<N_ROWS> > rbs, rzs;
static std::vector< PackedZ2<N_COLS> > rxs;

void PRF_unpacked_central(std::vector<uint64_t>& K1, PackedZ2<N_COLS>& x1, std::vector<uint64_t>& K2,
                          PackedZ2<N_COLS>& x2, std::vector< PackedZ3<81> >& Rmat, PackedZ3<81>& out1Z3,
                          PackedZ3<81>& out2Z3, int i)
{
    //1.perform X = x1+ x2 (on vectors)
    PackedZ2<N_COLS> X = x1; //declare a variable
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
    timer_unpacked_cent_p1 += (std::chrono::system_clock::now() - start_p1).count();

    uint64_t KtimesX[256];
    //convert outKX vector
    for(int cnt = 0; cnt < 256; cnt++)
    {
        KtimesX[cnt] = outKX.at(cnt);
    }


    /*
     * Uncomment this part to perform KX naively
     */
    //3a. Unpack K and X and perform their multiplication using naive method
    uint64_t X_unpack[256];
    for(int i = 0; i < N_COLS;i++)
    {
        X_unpack[i] = (uint64_t)X.at(i);
    }
    uint64_t randMat[81][256];

    auto start_p2 = chrono::system_clock::now();
    //3b. Take outKX and multiply it with 81X256 Z3 matrix
    int nColsGenerated;
    int wLen = 64;
    int rows = 0;

    while(rows < 81) {
        nColsGenerated = 0;
        while (nColsGenerated < 256) {
            uint64_t wGen = randomWord();
            //k is the index within the wGen, which holds a random 64 bit word
            for (int k = 0; k < wLen; k = k + 2) {
                uint64_t bit1 = (wGen >> k) & 1;
                uint64_t bit2 = (wGen >> (k + 1)) & 1;
                if (!((bit1 == 1) & (bit2 == 1)))
                {
                    //assign the next bits generated to their locations in the random matrices, the location is nBitsFound
                    randMat[rows][nColsGenerated]=(bit1<<1 | bit2);
                    nColsGenerated++;
                    //if we reached the end of the random matrix word, we need to go to the next word. We will then generate a new random word to fill it
                    if (nColsGenerated == wLen) {
                        break; //go to the next item
                    }
                }
            }
        }
    rows++;
    }
    timer_unpacked_cent_p2 += (std::chrono::system_clock::now() - start_p2).count();

    //4.Multiply KtimesX with K_unpack

    int out_mod3[81];
    auto start_p3 = chrono::system_clock::now();
    for(int i = 0; i < 81; i++)
    {
        int sum = 0;
        for(int j = 0; j < 256; j++)
        {
           sum += KtimesX[j] * randMat[i][j];
        }
        out_mod3[i] = sum % 3;
    }
    timer_unpacked_cent_p3 += (std::chrono::system_clock::now() - start_p3).count();

    /*
    cout<<std::endl;
    //printing the generated randomization matrix
    for(int i = 0;i<81;i++)
    {
        for(int j = 0;j < 256; j++)
        {
            cout<<K_unpack[i][j];
        }
        cout<<std::endl;
    }*/
}

void PRF_unpacked_driver(int nTimes,  int nRuns, int nStages)
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
        PRF_unpacked_central(K1, x1, K2, x2, Rmat, out1Z3, out2Z3, i);
    }
}

void display_time_unpacked(int nRuns)
{
    std::cout<<std::endl<<"Time to execute unpacked phase 1 in ms for "<<nRuns << " runs = " << timer_unpacked_cent_p1<<  std::endl;
    std::cout<<"Time to generate randomization matrix in ms for "<<nRuns << " runs = " <<timer_unpacked_cent_p2 << std::endl;
    std::cout<<"Time to execute unpakced phase 3 in ms for "<<nRuns << " runs = " <<timer_unpacked_cent_p3 << std::endl;
}