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

using namespace std;

#ifdef TEST_PRF

// This test program implements the first phase of the PRF
//int main(int argc,char* argv[] ) {
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

    PRF_DM(ntimes, nRuns, stepsToRun);

}

#endif



#ifdef TEST_SC
// This test program implements the first phase of the PRF
int main()
{

    randomWord(1); // use seed=1

    preProc_OT(1); //preprocess for OT, generate ra, rn, rx, and z

    PackedZ2<N_SIZE> y1, y2;
    PackedZ3<N_SIZE> out1, out2;

    y1.randomize();
    y2.randomize();

    auto ySum = y1;
    ySum ^= y2;

    std::vector<unsigned int> arrayMod2;
    ySum.toArray(arrayMod2);

    SC_Party2_1(y2, /*index=*/0);
    SC_Party1(y1, out1, /*index=*/0);
    SC_Party2_2(y2, out2, /*index=*/0);

    std::vector<unsigned int> out1arrayMod3;
    std::vector<unsigned int> out2arrayMod3;
    out1.toArray(out1arrayMod3);
    out2.toArray(out2arrayMod3);

    auto z = out1;
    z += out2;
    std::vector<unsigned int> arrayMod3;
    z.toArray(arrayMod3);

    bool bTestPassed = false;
    if (arrayMod2 == arrayMod3)
        bTestPassed = true;

    cout << "SC test result = " << bTestPassed << endl;

    // Check the result
}

#endif

