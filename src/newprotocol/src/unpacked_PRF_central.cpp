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

long timer_PRF_phase1_packed = 0;
//long timer_PRF_phase1_unpacked = 0;
long timer_PRF_unpacked = 0;    //times the entire PRF protocol

void set_input(std::vector<uint64_t>& K, PackedZ2<N_COLS>& x)//setting up random input and key
{
    for (auto &w : K) w = randomWord();
    K[K.size() - 1] &= topelitzMask; // turn off extra bits at the end

    x.randomize();//random values as input(Z2)
}


//=========================================================

void PRF_unpacked_central(std::vector<uint64_t>& K, PackedZ2<N_COLS>& x, uint64_t randMat[81][256], uint64_t outMod3[81])
{

    std::chrono::time_point<std::chrono::system_clock> start_unpacked_prf, start_packed_p1;
    PackedZ2<N_COLS> outKX;//stores K*x in mod 2

    //start timing for the entire PRF
    start_unpacked_prf = chrono::system_clock::now(); //starting the clock for unpacked PRF(packed p1 and unpacked p3)

    //Performing K*x
    //start timing for the entire PRF
    start_packed_p1 = chrono::system_clock::now(); //starting the clock for phase 1 using packed
    outKX.toeplitzByVec(K,x);   //computes K * x
    timer_PRF_phase1_packed += (std::chrono::system_clock::now() - start_packed_p1).count();//stop phase 1 packed clock

    //convert outKX vector from mod 2 to mod 3
    uint64_t KtimesX[256];
    for(int cnt = 0; cnt < 256; cnt++)
    {
        KtimesX[cnt] = outKX.at(cnt);//convert from Z2 to Z3
    }

    for(int i = 0; i < 81; i++)
    {
        uint64_t sum = 0;
        for(int j = 0; j < 256; j++)
        {
           sum += KtimesX[j] * randMat[i][j];   //computing each element of the resultant column
        }
        outMod3[i] = sum % 3;   //performing mod 3 ocmputation
    }
    timer_PRF_unpacked += (std::chrono::system_clock::now() - start_unpacked_prf).count();

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

void display_time_unpacked(int nRuns)//displays the timing for unpacked PRF
{
    using Clock = std::chrono::system_clock;
    using Duration = Clock::duration;
    //std::cout << Duration::period::num << " , " << Duration::period::den << '\n';
    float time_unit_multiplier = 1;
    if(Duration::period::den == 1000000000)
        time_unit_multiplier = 0.001; //make nanosecond to microsecond
    else if(Duration::period::den == 1000000)
        time_unit_multiplier = 1;   //keep the unit as microsecond

    std::cout<<"\nTime to execute phase 1 packed for "<<nRuns<<" runs = "<<(timer_PRF_phase1_packed  * time_unit_multiplier)<<" microseconds";
    //std::cout<<"\nTime to execute phase 1 unpacked for "<<nRuns<<" runs = "<<(timer_PRF_phase1_unpacked  * time_unit_multiplier)<<" microseconds";
    std::cout<<"\nTime to execute entire unpacked PRF for "<<nRuns<<" runs = "<<(timer_PRF_unpacked  * time_unit_multiplier)<<" microseconds";
    std::cout<<"\nNumber of rounds per second for entire unpacked PRF "<<(1000/(timer_PRF_unpacked*time_unit_multiplier)*1000000)<<std::endl;
}

void PRF_unpacked_driver(int nRuns)
{
    randomWord(1); // use seed=1
    vector<uint64_t> K(toeplitzWords);
    PackedZ2<N_COLS> x;
    uint64_t out_mod3[81];  //actual output of unpacked PRF
    uint64_t dummyoutmod3;  //dummy output for accurate timing purpose

    set_input(K,x);//setting random inputs in Z2

    //generating the random matrix
    auto start_randZ3 = chrono::system_clock::now();
    //3b. Take outKX and multiply it with 81X256 Z3 matrix
    int nColsGenerated;//no. of columns generated
    int wLen = 64;  //
    int rows = 0;   //row counter

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

    //Running the protocol for nRuns times
    for (int i = 0; i < nRuns; i++) {
        PRF_unpacked_central(K, x, randMat, out_mod3);//calling unpacked central function in unpacked_PRF_central.cpp
        for(int i = 0; i < 81; i++)
        {
            dummyoutmod3 += out_mod3[i];//dummy variable for accurate timing
        }
        //dummyoutmod3 += out_mod3;
    }

    for(int i = 0; i < 81; i++)
    {
        std::cout << out_mod3[i];//printing out the final value.
    }

    /*
     * The following lines of code are to time phase 1 without packing
     */
    std::cout<<std::endl<<"Timing phase 1 without packing"<<std::endl;
    //PRF_phase1_unpacked_timing();

    display_time_unpacked(nRuns);
}

