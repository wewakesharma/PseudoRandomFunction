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

uint64_t MaGlobal[4][256];
uint64_t MbGlobal[4];
uint64_t mxbitGlobal;
uint64_t MxGlobal[4];
uint64_t m0Global, m1Global;
uint64_t raGlobal, rbGlobal;
uint64_t rxGlobal, zGlobal;

/*
 * Get the Ma and Mb - should be an interactive function eventually - we will wait for the other party to send the protocol
 */
void getMaMb(uint64_t Ma[256][256], uint64_t Mb[256])
{
    Ma = MaGlobal;
    Mb = MbGlobal;

    //reset the global variables
    for (int iRow=0; iRow<256; iRow++)
        for (int jCol = 0; jCol < 256; jCol++)
        {
            MaGlobal[iRow][jCol] = 0;
            MbGlobal[iRow] = 0;
        }
}

/*
 *
 */
void getMx(uint64_t mX[4])
{
    for (int iRow = 0; iRow < 4; iRow++)
    {
            mX[iRow] = MxGlobal[iRow]; ;  //generate a random matrix
    }
}

void sendMx(uint64_t Mx[4])
{
    for (int iRow = 0; iRow < 4; iRow++)
    {
        MxGlobal[iRow] = Mx[iRow];  //generate a random matrix
    }
}

void sendMaMb(uint64_t Ma[4][256], uint64_t Mb[4])
{
    for (int iRow=0; iRow < 4; iRow++) {
        for (int jCol = 0; jCol < 256; jCol++) {
            MaGlobal[iRow][jCol] = Ma[iRow][jCol];
        }
        MbGlobal[iRow] = Mb[iRow];
    }
}

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


void multProtP1(uint64_t A[4][256], uint64_t B[4], uint64_t Ra[4][256], uint64_t Rb[4], uint64_t out[4])
{
    uint64_t Mx[4];

    //wait to get mx
    getMx(Mx);
    //calculate and output
    uint64_t Ma[4][256];

    for (int iRow = 0; iRow < 4; iRow++)
        for (int jCol = 0; jCol < 256; jCol++) {
            Ma[iRow][jCol] = A[iRow][jCol] - Ra[iRow][jCol];
        }

    //multiply matrix with a vector
    uint64_t Mb_first[4];

     wordPackedVecMatMult(Ra,Mx,Mb_first);
     cout<<endl<<"Printing the z_final"<<endl;
     for(int i = 0; i< 4; i++)
     {
         cout<<Mb_first[i]<<endl;
     }

    //VecMatMultnotPack2(Ra,Mx,z_final);

    uint64_t Mb[4];

    for (int iCol = 0; iCol < 4; iCol++)
        Mb[iCol]= Mb_first[iCol] + B[iCol] - Rb[iCol];

    //send Ma and Mb to party 2

    sendMaMb(Ma,Mb);
    cout<<endl<<"Ma and Mb are sent to party 2"<<endl;
    out = B;//why is it so?
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
void multProtP2Part1(uint64_t X[4], uint64_t Rx[4], uint64_t Z[4], uint64_t out[4]) {
    uint64_t Mx[4];

    for (int iRow = 0; iRow < 4; iRow++)
        Mx[iRow] = X[iRow] - Rx[iRow];

    sendMx(Mx); //send the global status

}

void multProtP2Part2(uint64_t X[4], uint64_t Rx[4], uint64_t Z[4], uint64_t out[4])
{

    uint64_t Mx[4];

    uint64_t Ma[4][256];
    uint64_t Mb[4];

    getMx(Mx);   //get the global status

    getMaMb(Ma,Mb);

    uint64_t Ma_X[4];
    wordPackedVecMatMult(Ma,X,Ma_X);

    for (int i = 0; i < 2; i++)
        out[i] = Ma_X[i] + Mb[i] + Z[i];

}

void poly_eval(uint64_t A[4][256], uint64_t X[4], uint64_t B[4], uint64_t eval_out[4])
{
    uint64_t inter_poly_eval[4];//intermediate polynomial evaluation, the variable term
    wordPackedVecMatMult(A,X,inter_poly_eval);
    for(int cnt = 0; cnt < 4; cnt++)
    {
        eval_out[cnt] = inter_poly_eval[cnt] + B[cnt];
    }
}
/*
 * Rx and Z come from preprocessing
 * output = Ma*x+Mb+Z = (A-Ra)X + Ra(x-Rx) + B - RB + RaRx+Rb = AX+B
 *
 * to test this, we should have two threads, where one thread is waiting for the other thread to send it
 */

//void multProtP2(uint64_t X[256], uint64_t Rx[256], uint64_t Z[128], uint64_t out[128])
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



void sendmXbit(uint64_t mx)
{
    mxbitGlobal = mx;
}

void getmxbit(uint64_t mx)
{
    mx = mxbitGlobal;
}



/*
 * OT =
 *
 * from preprocessing:
 *
 * Z = ra rz + rb
 * rz is a bit
 * Za, ra,rb are elements in Z3
 *
 * unit test:
 * test that the output is r0 if x = 0 and r1 if x=1
 */



void sendm0m1(uint64_t m0,uint64_t m1)
{
    m0Global = m0;
    m1Global = m1;
}

void getm0m1(uint64_t m0,uint64_t m1)
{
    m0 = m0Global;
    m1 = m1Global;
}

/*
 * OT for the sender
 *  r0,r1,ra,rb are elements in Z_3
 *  mx are bits
 *
 * can be packed into vectors, but then we need two vectors - msb's and lsb's
 */

void OTS(uint64_t r0, uint64_t r1, uint64_t ra, uint64_t rb)
{
    uint64_t mx;
    uint64_t m0,m1;  //m0, m1 are in Z_3, the outpuf of this

    //getMx from P2, t
    getmxbit(mx);

    m0 = (mx * r1 + (1-mx) * r0 + rb) %3;


    m1 = ((1 - mx) * r1 + mx * r0 + ra + rb) %3;

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

    sendm0m1(m0,m1);

}

/*
 *
 * OT for the receiver, part 1
 *
 * x,rx are bits, Z is a Z3 components
 * w is in Z3 - the output
 *
 * Unit Test: check that w = ro if x=0, w = r1 if x=1
 * assign random input variables
 */

void OTRPart1(uint64_t x, uint64_t rx, uint64_t z)
{
    uint64_t mx;
    uint64_t m0,m1;
    getm0m1(m0,m1);

    mx = x ^ rx;
    sendmXbit(mx);
}

/*
 *
 * OT for the receiver, part 1
 *
 * x,rx are bits, Z is a Z3 components
 * w is in Z3 - the output
 * m0, m1 are elements in z3
 */

uint64_t OTRPart2(uint64_t x, uint64_t rx, uint64_t z)
{
    uint64_t m0,m1;
    getm0m1(m0,m1);

    uint64_t w = (rx * m1 + (1-rx) * m0 - z) % 3;

    return w;

    //check that this is equivalent to:
//    if (x==rx) {
//        if (x == 0)
//            w = m0 - z;
//        else
//            w = m1 - z;
//    } else {
//        if (x==0)
//            w = m1 - z;
//        else
//            w = m0 - z;
//
//    }
}

/*
 * x, rx is a bit
 *
 * This part requires 2 threads - one for the receiver that sends the data, waits for the data from the sender, and then continues
 * and the other thread for the sender.
 *
 *  * x,rx are bits, Z is a Z3 components
 * w is in Z3 - the output
 *
 * Can be packed into bits, but the calculations are mod 3
 */
void OTR(uint64_t x, uint64_t rx, uint64_t z, uint64_t w)
{
    uint64_t mx;
    uint64_t m0,m1;
    getm0m1(m0,m1);

    mx = x ^ rx;
    sendmXbit(mx);

    w = (rx * m1 + (1-rx) * m0 - z) % 3;

    //check that this is equivalent to:
//    if (x==rx) {
//        if (x == 0)
//            w = m0 - z;
//        else
//            w = m1 - z;
//    } else {
//        if (x==0)
//            w = m1 - z;
//        else
//            w = m0 - z;
//
//    }
}

void getrarb(uint64_t ra, uint64_t rb)
{
    ra = raGlobal;
    rb = rbGlobal;
}

void setrarb(uint64_t ra, uint64_t rb)
{
raGlobal = ra;
rbGlobal = rb;
}

/*
 * Get rx and z from preprocessing
 * rx, z are elements in Z3
 */
void getrxz(uint64_t rx, uint64_t z)
{
rx = rxGlobal;
z = zGlobal;
}

/*
 * rx and z are set during preprocessing
 */
void setrxz(uint64_t rx, uint64_t z)
{
rxGlobal = rx;
zGlobal = z;
}

/*
 * we have y1 + y2 = y mod 2
 * y1 is a bit
 * right now non-packed
 * eventually will not be non-packed
 * get ra, rb from processing
 *
 * r1, rb aer elements in Z3
 *
 *  * Unit testing should verify that (v+w) mod 3 = (y1 + y2) mod 2
 */

uint64_t sc23_p1(uint64_t y1, std::mt19937 &generator )
{
    uint64_t ra, rb, r0, r1;

    getrarb(ra,rb); //get, ra, rb fro preprocessing, both are elements in Z3

    //choose r from z3 random
    uint64_t r = generator() & 1; //for testing purposes

    r0 = (r + y1) % 3;
    r1 = (r + 1 - y1) % 3;

    //call the ORS of the sender
    OTS(r0,r1,ra,rb);

    uint64_t v = 3-r;
    return v;

    //run OT
}

/*
 * rx, z are binary elements, from preprocessing
 * output is in Z3
 * Unit testing should verify that (v+w) mod 3 = (y1 + y2) mod 2
 */

void sc23_p2Part1(uint64_t y2)
{
    uint64_t rx, z;

    uint64_t ra, rb, r0, r1;

    getrxz(rx,z); //get them from preprocessing

    //call the OR of the receiver - part 1
    OTRPart1(y2,rx,z);

    //run OT

}

/*
 * sc23_p2Part1 needs to run first, and then sc23_p1
 */
uint64_t sc23_p2Part2(uint64_t y2)
{
    uint64_t rx, z;

    uint64_t ra, rb, r0, r1;

    getrxz(rx,z); //get them from preprocessing

    //call the OR of the receiver - part 1
    uint64_t w = OTRPart2(y2,rx,z);

    return w;

    //run OT

}

/*
 * This part requires two threads - the OTR is running in one thread and the ORS in another one, the OTR waits until it gets back the output of the OTS
 */
void sc23_p2(uint64_t y2, uint64_t w )
{
    uint64_t rx, z;

    uint64_t ra, rb, r0, r1;

    getrxz(rx,z); //get them from preprocessing

    //call the OR of the receiver - part 1
    OTR(y2,rx,z,w);

    //run OT

}





void two_party_secret_share(uint64_t x1[4], uint64_t key1[4][256], uint64_t x2[4], uint64_t key2[4][256], uint64_t y[4])
{
    uint64_t z1[4];
    uint64_t z2[4];

    wordPackedVecMatMult(key1,x1,z1);
    wordPackedVecMatMult(key2,x2,z2);

    for (int i = 0; i < 4; i++)
        y[i] = z1[i] ^ z2[i];

}

//void two_party_gen_rand_input(uint64_t xin[4],uint64_t kin[4][256],uint64_t xout1[4],uint64_t xout2[4],uint64_t kout1[4][256],uint64_t kout2[4][256], std::mt19937 &generator)
//{
//    generate_rand_input(xout1,generator);
//    for (int i = 0; i < 4; i++)
//        xout2[i] = xin[i] ^ xout1[i];
//
//    generate_rand_matrix(kout1,generator);
//    for (int i = 0; i < 4; i++)
//        kout2[i] = kin[i] ^ xout2[i];
//
//};

void two_party_protocol()
{

}

/*
* each party calculates the share input and the shared key
        * each party runs first phase 1
* this function gets the outputs of all parties
        * b's are outputs of phase 1
* c's are chosen randomly - z_3 components
*/



 /*
  * how the y's are shared:
 * y_1 = x_2 * k_3 + x_3 * k_2 + x_3 * k_3
 */

void threePartyshareSecret(uint64_t b1[4], uint64_t b2[4], uint64_t b3[4], uint64_t cl[4], uint64_t cm[4], uint64_t dm, uint64_t dl)
{


};

