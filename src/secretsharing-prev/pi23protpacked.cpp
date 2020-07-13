#include <iostream>
#include <random>
#include <bitset>
#include <cmath>
#include <chrono> 
#include <ctime> 
#include <cstdlib>

//

#include "dmweakPRF.h"
#include "dmweakPRFpacked.h"
#include "pi23prot.h"
<<<<<<< HEAD
#include "utils.h"
=======
>>>>>>> cd6f466ade8684602c5c08e4f46c24663767b4c8
#include "pi23protpacked.h"

//unsigned long int z_final[4];//to store the final product values

using namespace std;


//these are global variables to be set to 0 at the beginning of the protocol, used to hold data transmitted between each party
extern uint64_t MaGlobal[2][4];
extern uint64_t MbGlobal[2];
extern uint64_t mxbitGlobal;
extern uint64_t MxGlobal[4];
extern uint64_t m0Global, m1Global;
extern uint64_t raGlobal, rbGlobal;
extern uint64_t rxGlobal, zGlobal;

uint64_t MxMulGlobalPacked[4];
uint64_t MxOTGlobalPacked[4];
uint64_t raPackedGlobalm[4], raPackedGloball[4], rbPackedGlobalm[4], rbPackedGloball[4];
uint64_t M0GlobalPackedm[4], M1GlobalPackedm[4];
uint64_t M0GlobalPackedl[4], M1GlobalPackedl[4];
uint64_t MaGlobalPacked[4][256];
uint64_t MbGlobalPacked[4];

void getInputPackedVars(uint64_t A1[4][256],uint64_t A2[4][256],uint64_t X1[4],
        uint64_t X2[4], std::mt19937 &generator) {
    generate_rand_packed_vector_4(X1, generator);
    generate_rand_packed_vector_4(X2, generator);
    generate_rand_packed_sqMat_4(A1, generator);
    generate_rand_packed_sqMat_4(A2, generator);
}


/*
 * Here we are generating binary vectors for the data
 *
 *     //Generate ahead of time RA, RB, RX, Z
    //RA = 4X256 packed matrix uint64_t[4][256]
    //RB, RX, Z = binary vectors, packed as uint64_t[4]
 */
void PreProcPackedGenVals(uint64_t Ra[4][256], uint64_t Rb[4], uint64_t Rx[4], uint64_t Z[4], std::mt19937 &generator)
    {
    generate_rand_packed_vector_4(Rx, generator);
    generate_rand_packed_sqMat_4(Ra, generator);
    generate_rand_packed_vector_4(Rb, generator);


//calculate Z = Ra*Rx+Rb
    for (int i = 0; i < 4; i++) {
        for (int jCol = 0; jCol < 256; jCol++) {
            Z[i] = (Ra[i][jCol] & Rx[jCol]) ^ Rb[jCol];
        }
    }
}

void getm0m1Packed(uint64_t m0m[4],uint64_t m0l[4], uint64_t m1m[4],uint64_t m1l[4])
{
    for (int i = 0; i < 4; i++) {
        m0m[i] = M0GlobalPackedm[i];
        m0l[i] = M0GlobalPackedl[i];
        m1m[i] = M0GlobalPackedl[i];
        m1l[i] = M1GlobalPackedl[i];
    }
}

void sendm0m1Z3Packed(uint64_t m0m[4],uint64_t m0l[4], uint64_t m1m[4],uint64_t m1l[4])
{
    for (int i = 0; i < 4; i++) {
        M0GlobalPackedm[i] = m0m[i];
        M0GlobalPackedl[i] = m0l[i];
        M1GlobalPackedm[i] = m1m[i] ;
        M1GlobalPackedl[i] = m1l[i] ;
    }
}


void sendMxOTPacked(uint64_t MxPacked[4])
{
    for (int iRow = 0; iRow < 4; iRow++)
    {
        MxOTGlobalPacked[iRow] = MxPacked[iRow];  //generate a random matrix
    }
}

void getMxOTPacked(uint64_t mX[4])
{
    for (int iRow = 0; iRow < 4; iRow++)
    {
        mX[iRow] = MxOTGlobalPacked[iRow]; ;  //generate a random matrix
    }
}

void sendMxMulPacked(uint64_t MxPacked[4])
{
    for (int iRow = 0; iRow < 4; iRow++)
    {
        MxMulGlobalPacked[iRow] = MxPacked[iRow];  //generate a random matrix
    }
}

void getMxMulPacked(uint64_t mX[4])
{
    for (int iRow = 0; iRow < 4; iRow++)
    {
        mX[iRow] = MxMulGlobalPacked[iRow]; ;  //generate a random matrix
    }
}



void getrarbPacked(uint64_t ram[4], uint64_t ral[4], uint64_t rbm[4], uint64_t rbl[4])
{
    ram = raPackedGlobalm;
    ral = raPackedGloball;
    rbm = rbPackedGlobalm;
    rbl = rbPackedGloball;
}



/*
 * Rx and Z come from preprocessing
 * output = Ma*x+Mb+Z = (A-Ra)X + Ra(x-Rx) + B - RB + RaRx+Rb = AX+B
 *
 * UnitTesting:
 *
 * call multProtP2Part1
 * Call MultProtP1
 * call MultProtP2Part2
 *
 *
 */



/*
 * The two parties hold an additive sharing of the key A and the input x
 * goal: compute additive sharing of max(aX)
 *
 * output: y' = Ax
 *
 * //at this point the inputs will be chosen randomly
 * Input should be matrices of bits
 * Used to calculate k1X2 and k2X1, as part of the calculation of (x1+x2)(K1+K2)
 *
 * send mx, receive mx
 * Needs to be optimized where we only care about the first column of X and the result
 *
 * Two functions - each for each party
 *
 * mx = 128x256
 *
 * In the preprocessing: Z is calculated using Ra, Rb and Rx
 * Z = Ra*Rx+Rb
 *
 * party 1 and 2 can change roles depending on the protocol phase
 *
 * Alice has K1,X1 and Bob has K2, X2
 *
 * Alice plays P1, K1 = A,
 *
 * When is it used:
 * when calculating (K1+K2)(X1+X2) = K1X1 (local calc) + K1 (from Alice) X2 (from Bob) + K2 (from Alice) X1 (from Bob) + K2X2 (local calc)
 * The second and third parts are calculated using this function
 *
 * * For example, to calculate: K1X2: Bob chooses B at random
 * B will be chosen at random outside this function, the function will get B
 *
 * Unit test:
 * test that the sum of the outputs is equal to AX, where A is a matrix of P1 and X is a vector of P2
 *
 * Choose random variables for all the variables
 *

 */


<<<<<<< HEAD
void AXplusB_P1Packed(uint64_t A[4][256], uint64_t B[4], uint64_t Ra[4][256], uint64_t Rb[4])
=======
void multProtP1Packed(uint64_t A[2][4], uint64_t B[2], uint64_t Ra[2][4], uint64_t Rb[2], uint64_t out[2])
>>>>>>> cd6f466ade8684602c5c08e4f46c24663767b4c8
{
    uint64_t Mx[4];

    //wait to get mx
    getMxMulPacked(Mx);
    //calculate and output
<<<<<<< HEAD
    uint64_t Ma[4][256];
=======
    uint64_t Ma[2][4];
>>>>>>> cd6f466ade8684602c5c08e4f46c24663767b4c8

    for (int iWord = 0; iWord < 4; iWord++)
        for (int jCol = 0; jCol < 256; jCol++) {
            Ma[iWord][jCol] = A[iWord][jCol] ^ Ra[iWord][jCol];
        }

    //multiply matrix with a vector
    uint64_t z_final[4];
    wordPackedVecMatMult(Ra,Mx,z_final);

<<<<<<< HEAD
    uint64_t Mb[4];

    for (int iCol=0; iCol<4; iCol++)
        Mb[iCol]= z_final[iCol] ^ B[iCol] ^ Rb[iCol];
    //z_final[iCol] + B[iCol] - Rb[iCol];
=======
    uint64_t Mb[2];

    for (int iCol = 0; iCol < 2; iCol++)
        Mb[iCol]= z_final[iCol] + B[iCol] - Rb[iCol];
>>>>>>> cd6f466ade8684602c5c08e4f46c24663767b4c8

    //send Ma and Mb to party 2

    sendMaMb(Ma,Mb);
}

/*
 *multProtP2PackedPart1
 */
<<<<<<< HEAD
void AXplusB_P2PackedPart1(uint64_t X[4], uint64_t Rx[4], uint64_t Z[4]) {
    uint64_t MxMulPacked[4];

    for (int iRow = 0; iRow < 4; iRow++)
        MxMulPacked[iRow] = X[iRow] ^ Rx[iRow];

    sendMxMulPacked(MxMulPacked);//send the global status
}

void AXplusB_P2PackedPart2(uint64_t X[4], uint64_t Rx[4], uint64_t Z[4], uint64_t out[4])
=======
void multProtP2Part1Packed(uint64_t X[4], uint64_t Rx[4], uint64_t Z[2], uint64_t out[2]) {
    uint64_t Mx[4];

    for (int iRow = 0; iRow < 4; iRow++)
        Mx[iRow] = X[iRow] - Rx[iRow];

    sendMx(Mx); //send the global status

}

void multProtP2Part2Packed(uint64_t X[4], uint64_t Rx[4], uint64_t Z[2], uint64_t out[2])
>>>>>>> cd6f466ade8684602c5c08e4f46c24663767b4c8
{

    uint64_t Mx[4];

<<<<<<< HEAD
    uint64_t Ma[4][256];
    uint64_t Mb[4];
=======
    uint64_t Ma[2][4];
    uint64_t Mb[2];
>>>>>>> cd6f466ade8684602c5c08e4f46c24663767b4c8

    getMxMulPacked(Mx);   //get the global status

    getMaMb(Ma,Mb);

    uint64_t z_final[4];
    //wordPackedVecMatMult(Ma,X,z_final);

    for (int i = 0; i < 4; i++)
        out[i] = z_final[i] ^ Mb[i] ^ Z[i];

}

/*
 * This is implementation of OT packed in Z3
 * x,rx are bit elements {0,1}
 * Z is an element in Z3
 *
 * Each time the OT protocol is executed, we need to generate the following variables:
 * RA - uint64_t [4][256,
 * RB, RX,Z, uint64_t [4]
 * Ram, Ral, Rbm, Rbl, Zm, Zl, each of uint64_t [4][256]
 * Rz uint64_t[4]
*/
void OTZ3_R_Part1Packed(uint64_t x[4], uint64_t rx[4])
{
    uint64_t MxOTPacked[4];

    for (int i = 0; i < 4; i++) {
        MxOTPacked[i] = x[i] ^ rx[i];
        sendMxOTPacked(MxOTPacked);
    }
}
/*
 * w = output
 */
void OTZ3_R_Part2Packed(uint64_t Rx[4], uint64_t Zm[4], uint64_t Zl[4], uint64_t Wm[4], uint64_t Wl[4]) {

    uint64_t M0m[4], M0l[4], M1m[4], M1l[4];
    uint64_t tmpm[4], tmpl[4];

    getm0m1Packed(M0m, M0l, M1m, M1l);


    for (int i=0; i<4; i++)
    {
        tmpl[i] = (Rx[i] & M1l[i]) | (~Rx[i] & M0l[i]) ;// lbs
        tmpm[i] = (Rx[i] & M1m[i]) | (~Rx[i] & M0m[i]); // msb
        subMod3(Wm[i], Wl[i], tmpm[i], tmpl[i], Zm[i], Zl[i]);
    }
}


/*
 * OT for the sender
 *  r0,r1,ra,rb are elements in Z_3
 *  mx are bits
 *
 * can be packed into vectors, but then we need two vectors - msb's and lsb's
 *
 * We're defining ra as equal to the original ra+rb
 */
<<<<<<< HEAD

void OTZ3_S_Packed(uint64_t r0m[4], uint64_t r0l[4], uint64_t r1m[4], uint64_t r1l[4], uint64_t ram[4], uint64_t ral[4], uint64_t rbm[4], uint64_t rbl[4])
{
    uint64_t MxOT[4];
    uint64_t M0l[4],M0m[4],M1l[4],M1m[4];  //m0, m1 are in Z_3, the outpuf of this
    uint64_t t1l[4], t1m[4], t2l[4], t2m[4], t3l[4], t3m[4], t4l[4], t4m[4], t5m[4], t5l[4],t6m[4],t6l[4] ;
    //these are variables that will help us calculate our items

    getMxOTPacked(MxOT);

    for (int i = 0; i < 4; i++)
    {
        t1m[i] = MxOT[i] & r1m[i];
        t1l[i] = MxOT[i] & r1l[i];
        t2m[i] = r1m[i] ^ t1m[i]; //should be minus mod Z3 - is this correct?
        t2l[i] = t2l[i] ^ t2l[i];
        t3m[i] = MxOT[i] & r0m[i];
        t3l[i] = MxOT[i] & r0l[i];
        t4m[i] = r0m[i] ^ t3m[i];
        t4l[i] = r0l[i] ^ t3l[i];
        t5m[i] = t1m[i] ^ t4m[i];
        t5l[i] = t1l[i] ^ t4l[i];
        t6m[i] = t2m[i] ^ t3m[i];
        t6l[i] = t2l[i] ^ t3l[i];


    }

    addMod3vec4(t5m,t5l,rbm,rbl, M0m, M0l);
    addMod3vec4(t6m,t6l,ram,ral,M1l,M1l);


/*
    for (int i = 0; i < 4; i++) {
        M0[i] = (Mx[i] * r1[i] + (1 - Mx[i]) * r0[i] + rb[i]) % 3;
        M1[i] = ((1 - Mx[i]) * r1[i] + Mx[i] * r0[i] + ra[i] + rb[i]) % 3;
    }
    */
    //can be packed at a later date

    //check that this is equivalent to:
//    if (mx == 0) {
//        m0 = r0 + rb; //mod 3
//        m1 = r1 + ra + rb; // mod 3
//        }
//    else{
//        m0 = r1 + rb; // mod 3
//        m1 = r0 + ra + rb; // mod 3
//        }

    sendm0m1Z3Packed(M0m,M0l,M1m,M1l);

}




void sc23_p2Part1Packed(uint64_t Y2[4], uint64_t Rx[4] )
{

    //call the OR of the receiver - part 1
    OTZ3_R_Part1Packed(Y2, Rx);

    //run OT

}

void sc23_p2Part2Packed(uint64_t Rx[4], uint64_t Zm[4], uint64_t Zl[4], uint64_t Wm[4], uint64_t Wl[4] )
{
    OTZ3_R_Part2Packed(Rx,Zm, Zl, Wm,Wl);
        //call the OR of the receiver - part 1
    //run OT

}

void sc23_p1Packed(uint64_t y1[4], uint64_t vm[4],  uint64_t vl[4], std::mt19937 &generator)
{
    uint64_t ram[4], ral[4], rbm[4], rbl[4];
    uint64_t r0m[4];
    uint64_t r0l[4], r1m[4], r1l[4];

    uint64_t Rm[4], Rl[4];

    generate_rand_packed_vector_4(Rm, generator);
    generate_rand_packed_vector_4(Rl, generator);

    getrarbPacked(ram, ral, rbm, rbl ); //get, ra, rb fro preprocessing, both are elements in Z3

    //choose r from z3 random
    generate_rand_packed_vector_4(r0m, generator);

    //TODO; This calculation needs to be transformed to Z3 packed version
    /*
    r0 = (r + y1) % 3;
    r1 = (r + 1 - y1) % 3;
*/
    //call the ORS of the sender
    OTZ3_S_Packed(r0m,r0l,r1m,r1l,ram,ral,rbm,rbl);

    for (int i = 0; i < 4; i++) {
        vm[i] = 1 ^ Rm[i];
        vl[i] = 1 ^ Rl[i];
    }

    //run OT
}


=======
void generate_randkey_pi_protocol(uint64_t key[2][4], std::mt19937 &generator)
{
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            key[i][j] = generator();
        }
    }
}
void generate_two_word_randinput(uint64_t input[4], std::mt19937 &generator)
{
    //srand(time(NULL));
    for(int i = 0; i < 4; i++)
    {
        //Call the generator that generate a 64-but word each time
        input[i] = generator();
    }
}
>>>>>>> cd6f466ade8684602c5c08e4f46c24663767b4c8

