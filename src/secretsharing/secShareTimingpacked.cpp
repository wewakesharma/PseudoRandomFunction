#include "mains.h"
#include <iostream>
#include <random>
#include <bitset>
#include <cmath>
#include <chrono> 
#include <ctime> 
#include <cstdlib>

#include "dmweakPRF.h"
#include "utils.h"
#include "pi23prot.h"
#include "dmweakPRFpacked.h"
#include "secShareTiming.h"
#include "pi23protpacked.h"

//int wLen = 64;

//unsigned long int z_final[4];//to store the final product values

using namespace std;




void CalcEachPartyPackedSecret(uint64_t outP1[4], uint64_t out2P2[4], uint64_t A1[4][256], uint64_t X1[4],uint64_t Y1[256])
{

    uint64_t MultRes[4];

    wordPackedVecMatMult(A1,X1,MultRes);    //A1 * X1, packed

    for (int i = 0; i < 4; i++)
        Y1[i] = outP1[i]^out2P2[i]^MultRes[i];
}




#ifdef PRF_TIMING

//Step 1: Generate random input and store it in input array.
//Step 2: Generate 1024 uint64 numbers and store it in 4X256 size.
//Step 3: compute() function will calculate the value of zi's
int main(int argc,char* argv[] ) {


    int stepsToRun;

    if (argc > 1) {
        char *p;
        stepsToRun = strtol(argv[1], &p, 10);
    }
    else
        stepsToRun=3;

    packedTiming(stepsToRun);

    return 0;
}

#endif

void packedTiming(int stepsToRun)
{
    uint64_t A1[4][256];
    uint64_t A2[4][256];
    uint64_t B1[4];
    uint64_t B2[4];

    uint64_t Rx1[4];
    uint64_t Rx2[4];
    uint64_t Z1[4];;
    uint64_t Z2[4];

    uint64_t X1[4];
    uint64_t X2[4];

    uint64_t RA1[4][256];
    uint64_t RB1[4];
    uint64_t RA2[4][256];
    uint64_t RB2[4];

    uint64_t outA[4];  //output of Alice,
    uint64_t outB[4]; //output of Bob

    uint64_t Y1[4];
    uint64_t Y2[4];

    unsigned seed = 7;    // std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed); // mt19937 is a standard mersenne_twister_engine

    getInputPackedVars(A1,A2,X1,X2, generator);

    bool bInputUnkown = 1;

    //Generate ahead of time RA, RB, RX, Z
    //RA = 4X256 packed matrix uint64_t[4][256]
    //RB, RX, Z = binary vectors, packed as uint64_t[4]
    //Z = RA*RX+RB (mod 2)

    if (bInputUnkown) {
        PreProcPackedGenVals(RA1,RB1, Rx1, Z1, generator);
        PreProcPackedGenVals(RA2, RB2, Rx2, Z2, generator);
    }

    //we generate the B's randomly
    generate_rand_packed_vector_4(B1, generator);
    generate_rand_packed_vector_4(B2, generator);

    //Alice has A1, X1, Bob HAS A2, X2
    //Alice is P1 here, Bob is P2 here
    //B is chosen at random, this is the output of P1
    //outP2 is the output of Bob
    //B is the outpu of Alice

    //if input is known, the vectors are set to 0
    for (int i = 0; i < 4; i++) {
        outA[i] = 0;
        outB[i] = 0;
    }

    if (bInputUnkown) {
        AXplusB_P2PackedPart1(X2, Rx1, Z1);
        AXplusB_P1Packed(A1, B1, RA1, RB1);
        AXplusB_P2PackedPart2(X2, Rx1, Z1, outA);
    }

    //here Alice and Bob exchange roles, Alice is P2 and Bob is P1
    if (bInputUnkown) {
        AXplusB_P2PackedPart1(X1, Rx2, Z2);
        AXplusB_P1Packed(A2, B2, RA2, RB2);
        AXplusB_P2PackedPart2(X2, Rx1, Z1, outB);
    }

    //Alice
    CalcEachPartyPackedSecret(B1,outB, A1,X1,Y1);
    //Bob calculates his part
    CalcEachPartyPackedSecret(B2,outA,A2,X2,Y2);

/*
    //Here we start the next phase were we deal with Z3 numbers
    uint64_t ram[4], ral[4], rbm[4], rbl[4]; //these are Z3 vectors
    uint64_t Rz[4]; //a binary vector
    uint64_t unpackedVecra[256], unpackedVecrb[256];

    generate_rand_Z3_packed_Vec_4(ram, ral, unpackedVecra, generator);
    generate_rand_Z3_packed_Vec_4(rbm, rbl, unpackedVecrb, generator);
    generate_rand_packed_vector_4(Rz, generator);

    uint64_t zZ3m[4], zZ3l[4];

    for (int i = 0; i < 4; i++) {
        zZ3m[i] = (Rz[i] & ram[i]) ^ (((-1) ^ Rz[i]) & rbm[i]) ;
        zZ3l[i] = (Rz[i] & ral[i]) ^ (((-1) ^ Rz[i]) & rbl[i]) ;
    }
    */

    /////////////

    uint64_t Vm[4], Vl[4];
    uint64_t Wm[4], Wl[4];
    uint64_t Rxp23[4];

    OTPreroc(generator);
    sc23_p2Part1Packed(Y2, generator);
    sc23_p1Packed(Y1,Vm,Vl, generator);

    sc23_p2Part2Packed(Wm, Wl, generator);

}



