//
// Created by Vivek Sharma on 2/9/21.
//
#include <iostream>
#include <cstdlib>
#include "mains.hpp"
#include "unpacked_PRF_central.h"
#include "OT.hpp"
#include "Timing.hpp"
#include <chrono>
#include "packedMod2.hpp"
#include "Toeplitz-by-x.hpp"
#include <chrono>

using namespace std;

#ifdef TEST_UNPACKED_PHASE1

void PRF_phase1(PackedZ2<N_COLS>& unpacked_x, std::vector<PackedZ2<N_COLS>>& unpacked_K,
        PackedZ2<N_COLS>& unpacked_outKx)
{

    for(uint64_t outer_count = 0; outer_count < N_COLS; outer_count++)
    {
        uint64_t sum = 0;
        for(uint64_t inner_count = 0; inner_count < N_COLS; inner_count++)
        {
            sum += unpacked_K[outer_count].at(inner_count) * unpacked_x.at(inner_count);
        }
        unpacked_outKx.set(outer_count,sum);
    }

}

int main()
{
    long timer_PRF_phase1_unpacked = 0;
    PackedZ2<N_COLS> unpacked_x;
    std::vector<PackedZ2<N_COLS>> unpacked_K(N_COLS);
    PackedZ2<N_COLS> unpacked_outKx;

    std::chrono::time_point<std::chrono::system_clock> start_PRF_phase1_unpacked;

    //set random values in them
    unpacked_x.randomize();
    for(uint64_t K_count = 0; K_count < N_COLS; K_count++)
    {
        unpacked_K[K_count].randomize();
    }

    //performing matrix vector multiplication
    uint64_t dummy_outKx;
    dummy_outKx = 0;

    start_PRF_phase1_unpacked = chrono::system_clock::now();//start truly unpacked p1

    for(uint64_t run_count = 0; run_count < 1000; run_count++)
    {
        PRF_phase1(unpacked_x,unpacked_K,unpacked_outKx);
    }
    timer_PRF_phase1_unpacked = (std::chrono::system_clock::now() - start_PRF_phase1_unpacked).count();

    for(int i = 0; i < N_COLS; i++)
    {
        dummy_outKx += unpacked_outKx.at(i);//dummy variable for accurate timing
    }

    std::cout<<"Printing unpaced output KX without packing"<<unpacked_outKx<<std::endl;

    using Clock = std::chrono::system_clock;
    using Duration = Clock::duration;
    //std::cout << Duration::period::num << " , " << Duration::period::den << '\n';
    float time_unit_multiplier = 1;
    if(Duration::period::den == 1000000000)
        time_unit_multiplier = 0.001; //make nanosecond to microsecond
    else if(Duration::period::den == 1000000)
        time_unit_multiplier = 1;   //keep the unit as microsecond

    std::cout<<"\nTime to execute phase 1 unpacked for 1000 runs = "<<(timer_PRF_phase1_unpacked  * time_unit_multiplier)<<" microseconds";
    return 0;
}


#endif
