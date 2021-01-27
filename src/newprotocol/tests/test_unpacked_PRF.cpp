//
// Created by Vivek Sharma on 8/25/20.
//

#include <iostream>
#include <cstdlib>
#include "packedMod2.hpp"
#include "packedMod3.hpp"
#include "mains.hpp"
//#include "PRF.hpp"
#include "unpacked_PRF_central.h"
#include "OT.hpp"
#include "Timing.hpp"
#include <chrono>

using namespace std;

#ifdef UNPACKED_PRF_CENTRAL

int main()  {
    int ntimes, nRuns, stepsToRun;
    nRuns=1000;
    ntimes= 1;
    stepsToRun = 1;

    using Clock = std::chrono::system_clock;
    using Duration = Clock::duration;
    std::cout << Duration::period::num << " , " << Duration::period::den << '\n';

    PRF_unpacked_driver(ntimes, nRuns, stepsToRun); //driver program does the preprocessing and called unpacked function
    display_time_unpacked(nRuns);
    return 0;
}
#endif