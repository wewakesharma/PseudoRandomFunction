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
    int nRuns;
    nRuns=1000;

    PRF_unpacked_driver(nRuns); //driver program does the preprocessing and called unpacked function
    display_time_unpacked(nRuns);
    return 0;
}
#endif