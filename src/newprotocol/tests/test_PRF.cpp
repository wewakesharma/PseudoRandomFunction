// Copyright (C) 2020 Tzipora Halevi, MIT license - see LICENSE file
/** test_OT.cpp
 *  - testing the protocols for OT and share conversion
 */
#include <iostream>
#include <cstdlib>
#include "packedMod2.hpp"
#include "packedMod3.hpp"
#include "mains.hpp"
#include "PRF.hpp"
#include "OT.hpp"
#include "Timing.hpp"
#include <chrono>
#include "lookup_functions.h"

using namespace std;

#ifdef TEST_PRF

// This test program implements the first phase of the PRF

int main()  {
    int nRuns=1000;
    int stepsToRun = 1;
    int ntimes = 1;
#ifndef TEST_PRF_LOOKUP
    std::cout<<"Executing Darkmatter PRF protocol without lookup"<<std::endl;
#endif
#ifdef TEST_PRF_LOOKUP
    std::cout<<"Executing Darkmatter PRF protocol with lookup"<<std::endl;
#endif
    //calling the function in PRF.cpp that performs preprocesssing and execute the main protocol
    PRF_DM_wpreproc(ntimes, nRuns, stepsToRun);

}

#endif
