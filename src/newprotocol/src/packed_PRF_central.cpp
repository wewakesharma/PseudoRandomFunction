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
#include <chrono>

using namespace std;

long timer_PRF_packed = 0;//timing for entire PRF
long timer_packed_cent_p1 = 0;//timing for first phase, K*X
long timer_packed_cent_p2 = 0;//timing to measure share conversion from Z2 to Z3
long timer_packed_cent_p3 = 0;//timing to compute matrix vector computation between output of phase 2 and Randomization matrix


//sets random values to shares of input and key for two parties
void set_distributed_input(std::vector<uint64_t>& K1, PackedZ2<N_COLS>& x1, std::vector<uint64_t>& K2,
                           PackedZ2<N_COLS>& x2, std::vector< PackedZ3<81> >& Rmat)
{
    randomWord(1); // use seed=1

//randomize the matrix
    for (auto &col : Rmat) // iterate over the columns
        col.randomize();

// Choose random K1, K2, x1, x2, we will be computing
    for (auto &w : K1) w = randomWord();
    K1[K1.size() - 1] &= topelitzMask; // turn off extra bits at the end

    for (auto &w : K2) w = randomWord();
    K2[K2.size() - 1] &= topelitzMask; // turn off extra bits at the end

    x1.randomize();
    x2.randomize();     //randomize inputs
}

void PRF_packed_test(std::vector<uint64_t>& K1, PackedZ2<N_COLS>& x1, std::vector<uint64_t>& K2,
                          PackedZ2<N_COLS>& x2, std::vector< PackedZ3<81> >& Rmat, PackedZ3<81>& outZ3, int runs)
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
    PackedZ2<N_COLS> outKX; //stores the output of K*x in Z2

    auto start_prf = std::chrono::system_clock::now();//start the clock

    auto start_p1 = chrono::system_clock::now();//start the clock
    //Since K is a toeplitz, this function performs matrix vector multiplication where matrix is of kind toeplitz
    outKX.toeplitzByVec(K,X);
    timer_packed_cent_p1 += (std::chrono::system_clock::now() - start_p1).count();//time the clock and take difference from start_p1

    PackedZ3<256> outKX_Z3;//Z2 to Z3 converted output of phase 1

    PackedZ2<N_SIZE> hi; //MSB will be initialized to 0, since Z2 to Z3 won't affect the MSB's

    auto start_p2 = chrono::system_clock::now();
    outKX_Z3.makeFromBits(hi.bits, outKX.bits);//outKX_Z3 is a Z3 containing 0/1 values in Z2
    timer_packed_cent_p2 += (std::chrono::system_clock::now() - start_p2).count();//time the clock and take difference from start_p2

    //PackedZ3<81> outZ3;//final Z3 output
    auto start_p3 = chrono::system_clock::now();        //start the clock and save the time.
    outZ3.matByVec(Rmat,outKX_Z3);              //output of Rmat * (K * x); output of wPRF
    timer_packed_cent_p3 += (std::chrono::system_clock::now() - start_p3).count();//time the clock and take difference from start_p3

    timer_PRF_packed += (std::chrono::system_clock::now() - start_prf).count();

}

void PRF_packed(int nTimes,  int nRuns, int nStages)
{
    //Declare the input shares and key shares which will be distributed among two parties
    vector<uint64_t> K1(toeplitzWords), K2(toeplitzWords);
    PackedZ2<N_COLS> x1, x2;
    std::vector<PackedZ3<81> > Rmat(256); // generate a 81x256 matrix, publicly available

    PackedZ3<81> outZ3;                     // 81 bit vector in Z3, stores the output of PRF_packed_test
    PackedZ3<81> outZ3_dummy;       //dummy variable to make sure that output value is used.

    set_distributed_input(K1,x1,K2,x2,Rmat);//sets input with random values; in packed_PRF_central.cpp

    for (int i = 0; i < nRuns; i++) {
        PRF_packed_test(K1, x1, K2, x2, Rmat, outZ3, i);//calls the function in packed_PRF_central.cpp
        outZ3_dummy+=outZ3; //cumulative addition to make sure values are used and timing are more accurate.
    }
    std::cout<<"Output of packed_PRF_central is "<<outZ3<<  std::endl;
}

//Displays the timing after measuring unit metric for the machine it runs in.
void display_times(int nRuns)
{
    using Clock = std::chrono::system_clock;
    using Duration = Clock::duration;
    //std::cout << Duration::period::num << " , " << Duration::period::den << '\n';
    float time_unit_multiplier = 1;
    if(Duration::period::den == 1000000000)
        time_unit_multiplier = 0.001; //make nanosecond to microsecond
    else if(Duration::period::den == 1000000)
        time_unit_multiplier = 1;   //keep the unit as microsecond

    std::cout<<"Time for phase 1(K*X), "<<nRuns << " runs = " << (timer_packed_cent_p1 * time_unit_multiplier)<<" microseconds"<<  std::endl;
    std::cout<<"Time for phase 2(casting w from Z2 to Z3), "<<nRuns << " runs = " << (timer_packed_cent_p2 * time_unit_multiplier)<<" microseconds"<<  std::endl;
    std::cout<<"Time for phase 3(Randomization multiplication) "<<nRuns << " runs = " <<(timer_packed_cent_p3 * time_unit_multiplier)<<" microseconds" << std::endl;
    std::cout<<"Time for entire packed centralized PRF,  "<<nRuns << " runs = " <<(timer_PRF_packed  * time_unit_multiplier)<<" microseconds"<< std::endl;
    std::cout<<"Number of rounds per second for phase 1(K*X) "<<(1000/(timer_packed_cent_p1*time_unit_multiplier)*1000000)<<std::endl;
    std::cout<<"Number of rounds per second for phase 2(casting w from Z2 to Z3) "<<(1000/(timer_packed_cent_p2*time_unit_multiplier)*1000000)<<std::endl;
    std::cout<<"Number of rounds per second for phase 3(Randomization multiplication) "<<(1000/(timer_packed_cent_p3*time_unit_multiplier)*1000000)<<std::endl;
    std::cout<<"Number of rounds per second for entire packed centralized PRF "<<(1000/(timer_PRF_packed*time_unit_multiplier)*1000000)<<std::endl;
}