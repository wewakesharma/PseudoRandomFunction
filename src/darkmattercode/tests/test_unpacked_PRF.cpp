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

using namespace std;

#ifdef UNPACKED_PRF_CENTRAL

int main(int argc,char* argv[] )  {
    int stepsToRun, nRuns;
    if (argc>1){
        char *p;
        nRuns = strtol(argv[1], &p, 10);
    } else
        nRuns=1;

    if (argc > 2) {
        char *p;
        stepsToRun = strtol(argv[2], &p, 10);
    } else
        stepsToRun = 3;

    int ntimes = 1;
    nRuns = 1000; //Enable this line for 1000 runs

    PRF_unpacked_driver(ntimes, nRuns, stepsToRun); //driver program does the preprocessing and called unpacked function
    display_time_unpacked(nRuns);

}
#endif