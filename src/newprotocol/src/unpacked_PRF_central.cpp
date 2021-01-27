//
// Created by Vivek Sharma on 8/25/20.
//
#include <cassert>
#include "packedMod2.hpp"
#include "Toeplitz-by-x.hpp"
#include "OT.hpp"
#include "mains.hpp"
#include "Timing.hpp"
#include <chrono>


using namespace std;

long timer_PRF_unpacked = 0;    //times the entire PRF protocol


void PRF_unpacked_central(std::vector<uint64_t>& K1, PackedZ2<N_COLS>& x1, std::vector<uint64_t>& K2,
                          PackedZ2<N_COLS>& x2, uint64_t randMat[81][256], PackedZ3<81>& out1Z3,
                          PackedZ3<81>& out2Z3, int i, int outMod3[81])
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
    //start timing for the entire PRF
    std::chrono::time_point<std::chrono::system_clock> start_unpacked_prf = chrono::system_clock::now(); //starting the clock for unpacked
    //Performing K*x
    outKX.toeplitzByVec(K,X);

    //convert outKX vector
    uint64_t KtimesX[256];
    for(int cnt = 0; cnt < 256; cnt++)
    {
        KtimesX[cnt] = outKX.at(cnt);
    }

    uint64_t out_mod3[81];
    int out_mod3_dummy[81];
 //   uint64_t dummy;

    for(int i = 0; i < 81; i++)
    {
   //     dummy = 0;
        uint64_t sum = 0;
        for(int j = 0; j < 256; j++)
        {
           sum += KtimesX[j] * randMat[i][j];
        }
        out_mod3[i] = sum % 3;
   //     out_mod3_dummy[i] += out_mod3[i];
    //    dummy += out_mod3_dummy[i];

    }
    timer_PRF_unpacked += (std::chrono::system_clock::now() - start_unpacked_prf).count();;
   // std::cout<<dummy;

    //cout << "in PRF_unpacked_central" << "out_mod3=" << out_mod3 << endl;

#ifdef PRINT_VAL
    cout<<std::endl;
    //printing the generated randomization matrix
    for(int i = 0;i<81;i++)
    {
        for(int j = 0;j < 256; j++)
        {
            cout<<K_unpack[i][j];
        }
        cout<<std::endl;
    }
#endif
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

    for (auto &w : K1) w = randomWord();
    K1[K1.size() - 1] &= topelitzMask; // turn off extra bits at the end

    for (auto &w : K2) w = randomWord();
    K2[K2.size() - 1] &= topelitzMask; // turn off extra bits at the end

    x1.randomize();
    x2.randomize();

    PackedZ3<81> out1Z3;                     // 81-vector
    PackedZ3<81> out2Z3;                     // 81-vector
    PackedZ3<81> dummy;
    uint64_t dummyoutmod3;

    //generating the random matrix
    auto start_randZ3 = chrono::system_clock::now();
    //3b. Take outKX and multiply it with 81X256 Z3 matrix
    int nColsGenerated;
    int wLen = 64;
    int rows = 0;

    uint64_t randMat[81][256];

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

    uint64_t out_mod3[81];

    //TODO: write phase 1 function
    for (int i = 0; i < nRuns; i++) {
        PRF_unpacked_central(K1, x1, K2, x2, randMat, out1Z3, out2Z3, i, out_mod3);
        dummy += out1Z3;
        dummy += out2Z3;
        dummyoutmod3+=out_mod3;
    }

    std::cout<<dummy << dummyoutmod3;
}

void display_time_unpacked(int nRuns)
{
    using Clock = std::chrono::system_clock;
    using Duration = Clock::duration;
    //std::cout << Duration::period::num << " , " << Duration::period::den << '\n';
    float time_unit_multiplier = 1;
    if(Duration::period::den == 1000000000)
        time_unit_multiplier = 0.001; //make nanosecond to microsecond
    else if(Duration::period::den == 1000000)
        time_unit_multiplier = 1;   //keep the unit as microsecond
    std::cout<<"\nTime to execute entire unpacked PRF for "<<nRuns<<" runs = "<<(timer_PRF_unpacked  * time_unit_multiplier)<<" microseconds";
    std::cout<<"\nNumber of rounds per second for entire unpacked PRF "<<(1000/(timer_PRF_unpacked*time_unit_multiplier)*1000000)<<std::endl;
}