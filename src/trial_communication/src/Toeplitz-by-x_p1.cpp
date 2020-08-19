// Copyright (C) 2020 Tzipora Halevi, MIT license - see LICENSE file
/** Toeplitz-by-x.cpp
 *  - implementing a protocol for A*x (aka A*x+b) mod 2
 *    A is a Toeplitz matrix
 */
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <cassert>

#include "packedMod2.hpp"
#include "Toeplitz-by-x.hpp"
#include "data_sender.h"
#include "data_receiver.h"

using namespace std;
// in Toeplitz-by-x.hpp
// #define N_ROWS 256
// #define N_COLS 256

// This test program implements the first phase of the PRF

int main()
{
    randomWord(1); // use seed=1
    std::vector<uint64_t> K1(toeplitzWords), K2(toeplitzWords);
    PackedZ2<N_COLS> x1, x2;

    PackedZ2<N_ROWS> out1_A, out2_A, out1_B, out2_B;

    initGlobals(out1_A, K1);  // initialize some global variables
    preProc_Toeplitz_by_x(2); // pre-processing for two runs

    // Choose random K1, K2, x1, x2, we will be computing
    // (K1 xor K2) \times (x1 xor x2)

    for (auto& w : K1) w = randomWord();
    K1[K1.size()-1] &= topelitzMask; // turn off extra bits at the end

    for (auto& w : K2) w = randomWord();
    K2[K2.size()-1] &= topelitzMask; // turn off extra bits at the end

    x1.randomize();
    x2.randomize();

    //Initialize party 1 to recv the input
    //string recd_msg = pi_recv();
    //cout<<recd_msg<<endl;

    int port = 12345;
    std::vector<unsigned int> recd_mx = pi_recv(port);

    //printing the value of received mx.
    // First run, a protocol for K1 times x2

    int port2 = 54321;
    pi_snd(recd_mx,port2);
    //topelitz_Party1(out1_A, K1, 0);
    /*
    // Second run, a protocol for K2 times x1
    topelitz_Party2_1(x1, 0);
    topelitz_Party1(out2_B, K2, 0); // same FIXME as above
    topelitz_Party2_2(out2_A, x1, 0);

    // Party1 computes locally K1 times x1, and adds to out1_A,out2_A
    out1_A.add(out2_A);           // out1 ^= out2
    out2_A.toeplitzByVec(K1, x1); // K1 times x1
    out1_A.add(out2_A);           // sum of all terms

    // Party2 computes locally K2 times x2, and adds to out1_B,out2_N
    out1_B.add(out2_B);           // out1 ^= out2
    out2_B.toeplitzByVec(K2, x2); // K2 times x2
    out1_B.add(out2_B);           // sum of all terms

    // check the result

    out1_B.add(out1_A);                  // add the two shares

    x1.add(x2);                          // set x = x1 ^ x2
    for (unsigned i=0; i<K1.size(); i++) // Set K = K1 ^ K2
        K1[i] ^= K2[i];

    out1_A.toeplitzByVec(K1, x1);        // re-compute K*x locally

    if (out1_A == out1_B)
        std::cerr << "Yay, mult-by-topelit succeeded"<<std::endl;
    else
        std::cerr << "Boo, mult-by-topelit failed"<<std::endl;
        */
}
