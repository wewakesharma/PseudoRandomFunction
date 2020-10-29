//
// Created by Vivek Sharma on 8/24/20.
//

#include <iostream>
#include <cstdlib>
#include "packedMod2.hpp"
#include "packedMod3.hpp"
#include "mains.hpp"
//#include "PRF.hpp"
#include "packed_PRF_central.h"
#include "OT.hpp"
#include "Timing.hpp"
#include <chrono>

using namespace std;

#ifdef PACKED_PRF_CENTRAL

int main(int argc,char* argv[] )  {
    int stepsToRun, nRuns;
    if (argc>1){
        char *p;
        nRuns = strtol(argv[1], &p, 10);
    } else
        nRuns=1000;

    if (argc > 2) {
        char *p;
        stepsToRun = strtol(argv[2], &p, 10);
    } else
        stepsToRun = 3;

    int ntimes = 1;
    nRuns = 1000; //number of times the program runs

    std::cout<<"Running Packed PRF Centralized for "<<nRuns<<" times."<<std::endl;
    using Clock = std::chrono::system_clock;
    using Duration = Clock::duration;
    std::cout << Duration::period::num << " , " << Duration::period::den << '\n';

    PRF_packed(ntimes, nRuns, stepsToRun);
    display_times(nRuns);
}
#endif

