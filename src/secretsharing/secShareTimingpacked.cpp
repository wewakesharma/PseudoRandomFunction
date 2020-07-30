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
#include "utilspacked.h"
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






void packedTiming(int stepsToRun) {
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

    getInputPackedVars(A1, A2, X1, X2, generator);

    bool bInputUnkown = 1;

    //Generate ahead of time RA, RB, RX, Z
    //RA = 4X256 packed matrix uint64_t[4][256]
    //RB, RX, Z = binary vectors, packed as uint64_t[4]
    //Z = RA*RX+RB (mod 2)

    if (bInputUnkown) {
        PreProcPackedGenVals(RA1, RB1, Rx1, Z1, generator);
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

    uint64_t randMat1[2][256], randMat2[2][256];
    uint64_t randMatZ3[81][256]; //randMatZ3 holds the Z3 elements
    generate_rand_Z3_matrix_81x256(randMat1, randMat2, randMatZ3, generator); //generate

    //============================AX+B with separated module==========================  
    
    chrono::time_point<std::chrono::system_clock> start = chrono::system_clock::now();


    for (int i = 0; i < 1000000; i++) {

        if (bInputUnkown) {
            AXplusB_P2PackedPart1(X2, Rx1,Z1);
            AXplusB_P1Packed(A1, B1, RA1, RB1);
            AXplusB_P2PackedPart2(X2, Rx1,Z1,outA);
        }

        //here Alice and Bob exchange roles, Alice is P2 and Bob is P1
        if (bInputUnkown) {
            AXplusB_P2PackedPart1(X1, Rx2,Z2);
            AXplusB_P1Packed(A2, B2, RA2, RB2);
            AXplusB_P2PackedPart2(X1, Rx2,Z2,outB);
        }

        //Alice
        CalcEachPartyPackedSecret(B1, outB, A1, X1, Y1);
        //Bob calculates his part
        CalcEachPartyPackedSecret(B2, outA, A2, X2, Y2);

    }

    chrono::duration<double> elapsed_seconds = chrono::system_clock::now() - start;
    cout << endl << "elapsed time for 1M runs of AX+B phase(original):  " << elapsed_seconds.count() << "  s\n";
    

    //============================AX+B with separated module==========================

    //for party 1 generate 2 counts of 4X256 matrix and 2 counts of size 4 vectors
    getInputPackedVars(A1, RA1, B1, RB1, generator);
    getInputPackedVars(A2, RA2, B2, RB2, generator);
    chrono::time_point<std::chrono::system_clock> start_p1 = chrono::system_clock::now();


    for (int i = 0; i < 1000000; i++) {
        if (bInputUnkown) {
            AXplusB_P1Packed(A1, B1, RA1, RB1);
            AXplusB_P1Packed(A2, B2, RA2, RB2);
        }
}
    chrono::duration<double> elapsed_seconds_p1 = chrono::system_clock::now() - start_p1;
    cout << endl << "elapsed time for 1M runs of AX+B phase(party 1):  " << elapsed_seconds_p1.count() << "  s\n";


    generate_rand_packed_vector_4(X1,generator);
    generate_rand_packed_vector_4(Rx2,generator);
    generate_rand_packed_vector_4(X2,generator);
    generate_rand_packed_vector_4(Rx1,generator);

    chrono::time_point<std::chrono::system_clock> start_p2 = chrono::system_clock::now();

    for (int i = 0; i < 1000000; i++) {
        if (bInputUnkown) {
            AXplusB_P2PackedPart1(X2, Rx1,Z1);
            AXplusB_P2PackedPart2(X2, Rx1,Z1,outA);
            AXplusB_P2PackedPart1(X1, Rx2,Z2);
            AXplusB_P2PackedPart2(X1, Rx2,Z2, outB);
        }
    }
        
    chrono::duration<double> elapsed_seconds_p2 = chrono::system_clock::now() - start_p2;
    cout << endl << "elapsed time for 1M runs of AX+B phase(party2):  " << elapsed_seconds_p2.count() << "  s\n";

    chrono::time_point<std::chrono::system_clock> start_calc = chrono::system_clock::now();
    CalcEachPartyPackedSecret(B1, outB, A1, X1, Y1);
    CalcEachPartyPackedSecret(B2, outA, A2, X2, Y2);
    chrono::duration<double> elapsed_seconds_calc = chrono::system_clock::now() - start_p2;
    cout << endl << "elapsed time for 1M runs of AX+B phase(calc):  " << elapsed_seconds_calc.count() << "  s\n";

    //============================WordPackedVecMult==========================

    chrono::time_point<std::chrono::system_clock> start_wp = chrono::system_clock::now();
    uint64_t Mx[4];
    uint64_t Ra[4][256];
    uint64_t Ra_Mx[4];
    generate_rand_packed_sqMat_4(Ra, generator);
    generate_rand_packed_vector_4(Mx, generator);

    for (int i = 0; i < 1000000; i++) {
            wordPackedVecMatMult(Ra,Mx,Ra_Mx);
    }
        chrono::duration<double> elapsed_seconds_wp = chrono::system_clock::now() - start_wp;
    cout << endl << "elapsed time for 1M runs of wordPackedVecMatMult phase:  " << elapsed_seconds_wp.count() << "  s\n";
    
    //============================OT evaluation==========================

    uint64_t rx[4]; //number generated in OTPreproc
    uint64_t wm[4], wl[4], X[4], r0m[4], r0l[4], r1m[4], r1l[4];
    uint64_t ral[4], ram[4], rbl[4], rbm[4];//LSB and MSB of r0, r1, ra, rb
    uint64_t r0unpck[256], r1unpck[256];
    uint64_t zm[4], zl[4];

    OTPreproc(generator);
    generate_rand_packed_vector_4(X, generator);
    OT_fetch_preprocessed_values(ram, ral, rbm, rbl, rx, zm, zl);
    generate_rand_Z3_packed_Vec_4(r0m, r0l, r0unpck, generator);
    generate_rand_Z3_packed_Vec_4(r1m, r1l, r1unpck, generator);

    chrono::time_point<std::chrono::system_clock> start_OT = chrono::system_clock::now();

    for (int i = 0; i < 1000000; i++) {
        OTZ3_R_Part1Packed(X, rx);
        OTZ3_S_Packed(r0m, r0l, r1m, r1l, ram, ral, rbm, rbl);
        OTZ3_R_Part2Packed(rx, zm, zl, wm, wl);
    }

    chrono::duration<double> elapsed_seconds_OT = chrono::system_clock::now() - start_OT;
    cout << endl << "elapsed time for 1M runs of OT phase:  " << elapsed_seconds_OT.count() << "  s\n";

    //============================SC Timing code==========================
    uint64_t y1[4], vm[4], vl[4];

    generate_rand_packed_vector_4(Y2, generator);
    OTPreproc(generator);
    generate_rand_packed_vector_4(y1, generator);

    chrono::time_point<std::chrono::system_clock> start_SC = chrono::system_clock::now();
    for (int i = 0; i < 1000000; i++) {
        sc23_p2Part1Packed(Y2, generator);//perform Mx = Y2 ^ rx
        sc23_p1Packed(y1, vm, vl, generator);
        sc23_p2Part2Packed(wm, wl, generator);
    }
    chrono::duration<double> elapsed_seconds_SC = chrono::system_clock::now() - start_SC;
    cout << endl << "elapsed time for 1M runs of SC phase:  " << elapsed_seconds_SC.count() << "  s\n";
    //====================================================================
}

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


    /////////////

    uint64_t Vm[4], Vl[4];
    uint64_t Wm[4], Wl[4];
    uint64_t Rxp23[4];

    OTPreproc(generator);
    sc23_p2Part1Packed(Y2, generator);
    sc23_p1Packed(Y1, Vm, Vl, generator);

    sc23_p2Part2Packed(Wm, Wl, generator);

    //multiply the result of the secret sharing with a random Z3 matrix 81X256

    //generate the random Z3 81X256 matrix first

    uint64_t output_Am[2], output_Al[2], output_Bm[2], output_Bl[2];

    multMod3_Z3Mat_Z3Vec(randMat1, randMat2, Vm, Vl, output_Am, output_Al); // matrix-vector multiply mod 3
    multMod3_Z3Mat_Z3Vec(randMat1, randMat2, Wm, Wl, output_Bm, output_Bl); // matrix-vector multiply mod 3


    //at this point we have the Z3 outputs A1 and B1*/

#ifdef PRF_TIMING

//Step 1: Generate random input and store it in input array.
//Step 2: Generate 1024 uint64 numbers and store it in 4X256 size.
//Step 3: compute() function will calculate the value of zi's
int main(int argc,char* argv[] ) {
    int stepsToRun;
    if (argc > 1) {
        char *p;
        stepsToRun = strtol(argv[1], &p, 10);
    } else
        stepsToRun = 3;

    packedTiming(stepsToRun);
    return 0;
}
#endif