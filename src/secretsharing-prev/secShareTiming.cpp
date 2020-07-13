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

void getInputVars(uint64_t A1[256][256],uint64_t A2[256][256], uint64_t X1[256], uint64_t X2[256],  std::mt19937 &generator)

{
    generate_rand_sqMat_256(A1, generator);
    generate_rand_sqMat_256(A2, generator);
    generate_rand_vector_256(X1, generator);
    generate_rand_vector_256(X2, generator);
}


void CalcEachPartyPackedSecret(uint64_t outP1[4], uint64_t out2P2[4], uint64_t A1[4][256], uint64_t X1[4],uint64_t Y1[256])
{

    uint64_t MultRes[4];

    wordPackedVecMatMult(A1,X1,MultRes);

    for (int i = 0; i < 4; i++)
        Y1[i] = outP1[i]^out2P2[i]^MultRes[i];
}

void CalcEachPartySecret(uint64_t outP1[256], uint64_t out2P2[256], uint64_t A1[256][256], uint64_t X1[256],uint64_t Y1[256])
{

    uint64_t MultRes[256];

    VecMatMultnotPack(A1,X1,MultRes);

    for (int i = 0; i < 256; i++)
        Y1[i] = outP1[i]^out2P2[i]^MultRes[i];
}

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

    uint64_t outP1[4];
    uint64_t outP2[4];
    uint64_t out2P1[4];
    uint64_t out2P2[4];

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
    if (bInputUnkown) {
        AXplusB_P2PackedPart1(X2, Rx1, Z1);
        AXplusB_P1Packed(A1, B1, RA1, RB1);
        AXplusB_P2PackedPart2(X2, Rx1, Z1, outP2);
    }

    //here Alice and Bob exchange roles, Alice is P2 and Bob is P1
    if (bInputUnkown) {
        AXplusB_P2PackedPart1(X1, Rx2, Z2);
        AXplusB_P1Packed(A2, B2, RA2, RB2);
        AXplusB_P2PackedPart2(X2, Rx1, Z1, out2P2);
    }

    //Alice
    CalcEachPartyPackedSecret(B1,out2P2, A1,X1,Y1);
    //Bob calculates his part
    CalcEachPartyPackedSecret(B2,outP2,A2,X2,Y2);

    //Here we start the next phase were we deal with Z3 numbers
    uint64_t ram[4], ral[4], rbm[4], rbl[4]; //these are Z3 vectors
    uint64_t Rz[4]; //a binary vector
    uint64_t unpackedVecra[256], unpackedVecrb[256];

    generate_rand_Z3_packed_Vec_4(ram, ral, unpackedVecra, generator);
    generate_rand_Z3_packed_Vec_4(rbl, rbl, unpackedVecrb, generator);
    generate_rand_packed_vector_4(Rz, generator);

    uint64_t zZ3m[4], zZ3l[4];

    for (int i = 0; i < 4; i++) {
        zZ3m[i] = (Rz[i] & ram[i]) ^ ((1-Rz[i]) & rbm[i]) ;
        zZ3l[i] = (Rz[i] & ral[i]) ^ ((1-Rz[i]) & rbl[i]) ;
    }

    /////////////

    uint64_t Vm[4], Vl[4];
    uint64_t Wm[4], Wl[4];
    uint64_t Rxp23[4];
    uint64_t Zmp23[4], Zlp23[4];
    generate_rand_packed_vector_4(Rxp23, generator);

    sc23_p2Part1Packed(Y2, Rxp23);
    sc23_p1Packed(Y1,Vm,Vl, generator);

    sc23_p2Part2Packed(Rxp23, Zmp23, Zlp23, Wm, Wl);

}


void UnpackedTiming(int stepsToRun)
{
    unsigned seed = 7;    // std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed); // mt19937 is a standard mersenne_twister_engine

    uint64_t key[4][256];
    //randMat1 holds the LSB's, randMat2 holds the MSB's of the randomization matrix
    uint64_t randMat1[2][256], randMat2[2][256];
    uint64_t randMatZ3[81][256]; //randMatZ3 holds the Z3 elements
    uint64_t input[4];
    uint64_t outM[2];
    uint64_t outL[2];
    uint64_t output_p1[4];
    uint64_t output_p3[84];



    uint64_t X1[256];
    uint64_t X2[256];
    uint64_t Rx1[256];
    uint64_t Rx2[256];
    uint64_t Z1[256];
    uint64_t Z2[256];

    uint64_t A1[256][256];
    uint64_t A2[256][256];
    uint64_t B1[256];
    uint64_t B2[256];
    uint64_t Ra1[256][256];
    uint64_t Rb1[256];
    uint64_t Ra2[256][256];
    uint64_t Rb2[256];
    uint64_t outP1[256];
    uint64_t outP2[256];
    uint64_t out2P1[256];
    uint64_t out2P2[256];
    uint64_t Y1[256];
    uint64_t Y2[256];

    bool bInputUnkown = 1;

    //first start with choosing a default key and input
    //here both the input and key are secret shared

    //preprocessing happens here

    //this is only needed if the input is secret-shared

    //we first secret share the parameters

    //choose the secret key K1, K2 at random, and define K as their XOR
    //X comes from the application, we will choose X1 and X2 randomly, and X1 XOR X2 = X (this is secret sharing mod 2

    //here we get one application of the AX+B protocol
    //here we multiply K1*X2, where A = K1, X = X2

    //PROT1

    //Get Input Vars
    getInputVars(A1,A2,X1,X2, generator);
    //Ra, Rb are part of the preprocessing
    //Rx and Z = Ra*Rx=Rb, this is the random correlation
    //B's are chosen at random
    if (bInputUnkown) {
        PreProcGenVals(Ra1, Rb1, Rx1, Z1, generator);
        PreProcGenVals(Ra2, Rb2, Rx2, Z2, generator);
    }

    //we generate the B's randomly
    generate_rand_vector_256(B1, generator);
    generate_rand_vector_256(B2, generator);

    //we do not need to time the preprocessing

    if (bInputUnkown) {
        AXplusB_P2Part1(X2, Rx1, Z1);
        AXplusB_P1(A1, B2, Ra1, Rb1, outP1);
        AXplusB_P2Part2(X2, Rx1, Z1, outP2);
    }

    //PROT2
    //here we use K2, X1 - the parties roles are reversed
    //A = K2, X = X1
    //here the roles are reversed, so Alice, the party has has A1,X1 will play "party 2"
    if (bInputUnkown) {

        AXplusB_P2Part1(X1, Rx2, Z2);
        AXplusB_P1(A2, B2, Ra2, Rb2, out2P1);
        AXplusB_P2Part2(X1, Rx2, Z2, out2P2);
    }

    //now we have secret sharing of (K1 * X2) and (K2 * X1)

    //Party 1 now calculates Y1 = X1 * K1 (use the packed multiplication) and adds PROT1 party 1 output and PROT2 party 2 output
    //party 2 now calculates Y2 = X2 * K2 (use the packed multiplication) and adds PROT1 party 2 output and PROT2 party 1 output

    CalcEachPartySecret(outP1,out2P2, A1,X1,Y1);
    CalcEachPartySecret(outP2,out2P1,A2,X2,Y2);

    //now we have secret sharing mod 2 of K * X



    //at this point we call the secret sharing functions sc23_p1, sc23_p1
    //We use Y1 and Y2 that we just got
    //after this each party has a Z3 vector which is represented as outZ3-lsb and outZ3-msb


    uint64_t V[256];
    uint64_t W[256];
    uint64_t R[256];
    generate_rand_vector_256(R, generator);

    for (int i = 0; i < 256; i++) {
        sc23_p2Part1bit(Y1[i]);
        V[i] = sc23_p1Bit(Y2[i],R[i]);
        W[i] = sc23_p2Part2bit(Y1[i]);
    }

    //to check that the result is correct:
    //we calculate PRFOut = K*X
    //For each party, we calculate: OutZ3VP1 and OutZ3VP2 (by adding the msb and lsb)
    //we then add OutZ3VP1 and OutZ3VP2 mod 3 and compare to make sure it is equal to PRFOut

    //


    //choose a random key, random input

    //then, run the first phase

    //run the second phase

    //third phase

    //repeat and time this


  //  cout << "Please enter the number of steps to run    : ";
  //  cin >> stepsToRun;
    cout << "The number of phases to run you entered is " << stepsToRun << endl;

/*
	generate_rand_key(key, generator);

	//we generate two random matrices, one holds the first bit and one the second bit
	generate_rand_Z3_matrix(randMat1, randMat2, randMatZ3, generator);
    generate_rand_input(input,generator);

    //call once for testing purposes
    wordPackedVecMatMult(key,input, output_p1); // matrix-vector multiply mod 2

    uint64_t c[4], d[4];

    //shareSecret( output_p1,  c,  d)


    multMod3(outM, outL, randMat1, randMat2, output_p1); // matrix-vector multiply mod 3


    InnerProdMul(output_p3, randMatZ3, output_p1);  //multiply with integer packing
    InnerProdMul2(output_p3, randMatZ3, output_p1);  //multiply with integer packing

    //need to compare here both outputs and check that they are equal

    char p2output[256];

    //chrono::duration<double> elapsed_seconds1 ;
    //chrono::duration<double> elapsed_seconds2 ;
    //chrono::duration<double> elapsed_seconds3 ;

    chrono::time_point<std::chrono::system_clock> start = chrono::system_clock::now();

    for(int i=0;i<1000000;i++){

        wordPackedVecMatMult(key,input, output_p1); // matrix-vector multiply mod 2

        if (stepsToRun>=2)
            unpackOutput(output_p1,p2output); // useless operation that should not be here
        // This is where the mod2->mod3 protocol should be
        if (stepsToRun>=3)
            multMod3(outM, outL, randMat1, randMat2, output_p1); // output phase 1 = input phase 3matrix-vector multiply mod 3
    }

    chrono::duration<double> elapsed_seconds = chrono::system_clock::now() - start;

    cout<<endl<<"output of first, second phase is "<< output_p1 << ',' << p2output << endl;
    cout<<endl<<"output msb,lsb is "<< outM << ',' << outL << endl;
    cout << endl<< "elapsed time for 1M runs:  " << elapsed_seconds.count() << "  s\n";

    //compare timing with the other multiplciation

    start = chrono::system_clock::now();

    for(int i=0;i<1000000;i++){

        wordPackedVecMatMult(key,input, output_p1); // matrix-vector multiply mod 2
        //unpackOutput(output_p1,p2output); // useless operation that should not be here
        // This is where the mod2->mod3 protocol should be
        InnerProdMul(output_p3, randMatZ3, output_p1);  //multiply with integer packing, output p1 = input p3
    }

    elapsed_seconds = chrono::system_clock::now() - start;

    cout<<endl<<"output is "<< output_p1[0] << endl;
    cout << "elapsed time for 1M runs for integer packing:  " << elapsed_seconds.count() << "  s\n";

    start = chrono::system_clock::now();

    for(int i=0;i<1000000;i++){

        wordPackedVecMatMult(key,input, output_p1); // matrix-vector multiply mod 2
        //unpackOutput(output_p1,p2output); // useless operation that should not be here
        // This is where the mod2->mod3 protocol should be
        InnerProdMul2(output_p3, randMatZ3, output_p1);  //multiply with integer packing, output p1 = input p3
    }

    elapsed_seconds = chrono::system_clock::now() - start;

    cout<<endl<<"output is "<< output_p1[0] << endl;
    cout << "elapsed time for 1M runs for integer packing2:  " << elapsed_seconds.count() << "  s\n";

 */

}
