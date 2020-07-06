#include <iostream>
#include <random>
#include <bitset>
#include <cmath>
#include <chrono> 
#include <ctime> 
#include <cstdlib>

//

#include "dmweakPRF.h"
#include "dmweakPRFnoPack.h"
#include "pi23prot.h"

//unsigned long int z_final[4];//to store the final product values

using namespace std;


//these are global variables to be set to 0 at the beginning of the protocol, used to hold data transmitted between each party
extern uint64_t MaGlobal[128][256];
extern uint64_t MbGlobal[128];
extern uint64_t mxbitGlobal;
extern uint64_t MxGlobal[256];
extern uint64_t m0Global, m1Global;
extern uint64_t raGlobal, rbGlobal;
extern uint64_t rxGlobal, zGlobal;




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


void multProtP1Packed(uint64_t A[128][256], uint64_t B[128], uint64_t Ra[128][256], uint64_t Rb[128], uint64_t out[128])
{
    uint64_t Mx[256];

    //wait to get mx
    getMx(Mx);
    //calculate and output
    uint64_t Ma[128][256];

    for (int iRow = 0; iRow < 128; iRow++)
        for (int jCol = 0; jCol < 256; jCol++) {
            Ma[iRow][jCol] = A[iRow][jCol] - Ra[iRow][jCol];
        }

    //multiply matrix with a vector
    uint64_t z_final[4];
    wordPackedVecMatMult(Ra,Mx,z_final);

    uint64_t Mb[128];

    for (int iCol=0; iCol<128; iCol++)
        Mb[iCol]= z_final[iCol] + B[iCol] - Rb[iCol];

    //send Ma and Mb to party 2

    sendMaMb(Ma,Mb);
    out = B;
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
void multProtP2Part1Packed(uint64_t X[256], uint64_t Rx[256], uint64_t Z[128], uint64_t out[128]) {
    uint64_t Mx[256];

    for (int iRow = 0; iRow < 256; iRow++)
        Mx[iRow] = X[iRow] - Rx[iRow];

    sendMx(Mx); //send the global status
}

void multProtP2Part2Packed(uint64_t X[256], uint64_t Rx[256], uint64_t Z[128], uint64_t out[128])
{

    uint64_t Mx[256];

    uint64_t Ma[128][256];
    uint64_t Mb[128];

    getMx(Mx);   //get the global status

    getMaMb(Ma,Mb);

    uint64_t z_final[4];
    wordPackedVecMatMult(Ma,X,z_final);

    for (int i = 0; i < 128; i++)
        out[i] = z_final[i] + Mb[i] + Z[i];

}

/*
 * Rx and Z come from preprocessing
 * output = Ma*x+Mb+Z = (A-Ra)X + Ra(x-Rx) + B - RB + RaRx+Rb = AX+B
 *
 * to test this, we should have two threads, where one thread is waiting for the other thread to send it
 */

//void multProtP2Packed(uint64_t X[256], uint64_t Rx[256], uint64_t Z[128], uint64_t out[128])
//{
//    uint64_t Mx[256];
//    uint64_t Ma[128][256];
//    uint64_t Mb[128];
//
//    for (int iRow=1; iRow<256; iRow++)
//        Mx[iRow] = X[iRow] - Rx[iRow];
//
//    sendMx(Mx);
//
//    //Here, another thread is needed, and we need to make sure Ma and Mb is received.
//
//    getMaMb(Ma,Mb);
//
//    uint64_t z_final[4];
//    wordPackedVecMatMult(Ma,X,z_final);
//
//    for (int i = 0; i < 128; i++)
//        out[i] = z_final[i] + Mb[i] + Z[i];
//
//}

/*
 * //////////////////////////////
 */


