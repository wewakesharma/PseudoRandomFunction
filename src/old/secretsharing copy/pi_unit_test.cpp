//
// Created by Vivek Sharma on 7/13/20.
//
#include <iostream>
#include <random>
#include <bitset>
#include <cmath>
#include <chrono>
#include <ctime>
#include <cstdlib>

#include "utils.h"
#include "utilspacked.h"
#include "pi23protpacked.h"
#include "pi_unit_test.h"
#include "dmweakPRFpacked.h"

using namespace std;
//==============================For Unit Testing of first three functions===================
void poly_eval_global(uint64_t A[4][256], uint64_t X[4], uint64_t B[4], uint64_t global_res[4])//AX+B evaluation
{
    uint64_t A_X[4];//intermediate polynomial evaluation, the term representation of A*X
    wordPackedVecMatMult(A,X,A_X);
    for(int cnt = 0; cnt < 4; cnt++)
        global_res[cnt] = A_X[cnt] ^ B[cnt];
}

void poly_eval_test(uint64_t out[4], uint64_t global_res[4])
{
    int test_flag = 0;
    for(int i =0;i<4;i++)
    {
        if(out[i] != global_res[i])
        {
            test_flag = 1;
            break;
        }

    }
    cout<<"AX+B polynomial evaluation test status========>";
    if(test_flag == 0)
        cout<<"Test Passed";
    else
        cout<<"Test failed";
}

/*
 * generate_msb_lsb_Z3() takes four arguments each of size 4.
 * The first parameter is generated Z3 value {0,1,2},
 * Following two parameters are its msb and lsb value respectively
 * The last parameter is the engine to generate the random bits.
 */
/*void generate_bits_4(uint64_t x_bits[4], std::mt19937 &generator)
{
    for(int i = 0; i < 4; i++)
    {
        x_bits[i] = generator() & 1;
    }
}
void generate_msb_lsb_Z3(uint64_t z3_value[4], uint64_t z3_msb[4], uint64_t z3_lsb[4], std::mt19937 &generator)
{
    //uint64_t rand_num = generator();
    int cnt = 0;
    uint64_t rand_num;
    while(cnt<4)
    {
         rand_num = generator()&3;
         if(rand_num < 3)//number should be 0,1, or 2
         {
             z3_value[cnt] = rand_num;
             z3_msb[cnt] = (rand_num>>1) & 1;  //AND with 10
             z3_lsb[cnt] = rand_num & 1; //AND with 01
             cout<<z3_value[cnt]<<"\t"<<z3_msb[cnt]<<"\t"<<z3_lsb[cnt]<<endl;
             cnt++;
         }
    }
}*/

//Debugging code
void OTPreproc_debug(uint64_t ram[4],uint64_t ral[4],uint64_t rbm[4],uint64_t rbl[4],uint64_t rx[4],uint64_t zm[4],uint64_t zl[4],std::mt19937 &generator)
{
    uint64_t unpackedVecra[256], unpackedVecrb[256];
    generate_test_Z3_packed_Vec_4(ram,ral,unpackedVecra, generator);
    generate_test_Z3_packed_Vec_4(rbm,rbl, unpackedVecrb, generator);

    generate_test_packed_vector_4(rx, generator);

    for (int i = 0; i < 4; i++) {
        zm[i] = (ram[i] & rx[i]) ^ (rbm[i] & (~rx[i]));
        zl[i] = (ral[i] & rx[i]) ^ (rbl[i] & (~rx[i]));
    }
}

void OT_test(uint64_t wm[4],uint64_t wl[4],uint64_t X[4],uint64_t r0m[4],uint64_t r0l[4],uint64_t r1m[4],uint64_t r1l[4])
{
    //cout<<endl<<"Printing Wm and Wl"<<endl;
    uint64_t X_bit, wm_bit, wl_bit, r0m_bit, r0l_bit, r1m_bit, r1l_bit;
    bool test_flag = 1;//test pass
    bool mismatch_x0 = 0;//mismatch when x = 0
    bool mismatch_x1 = 0;//mismatch when x = 1


    for(int i = 0; i < 1; i++)
    {
        uint64_t wm_word = wm[i];
        uint64_t wl_word = wl[i];
        uint64_t X_word = X[i];
        uint64_t r0m_word = r0m[i];
        uint64_t r0l_word = r0l[i];
        uint64_t r1m_word = r1m[i];
        uint64_t r1l_word = r1l[i];
        for (int jBit = 0; jBit < 64; jBit++) {
            //TODO: EXAMINE EACH BIT SEPARATELY OF WWordm, WWordl
            //and see that it matches either r0m, r0l or r1m, r1l
            //cepending on the value of the XWord'i bith
            X_bit = (X_word >> jBit) & 1;
            wm_bit = (wm_word >> jBit) & 1;
            wl_bit = (wl_word >> jBit) & 1;
            r0m_bit = (r0m_word >> jBit) & 1;
            r0l_bit = (r0l_word >> jBit) & 1;
            r1m_bit = (r1m_word >> jBit) & 1;
            r1l_bit = (r1l_word >> jBit) & 1;
            if((X_bit == 0) & ((wl_bit != r0l_bit) | (wm_bit != r0m_bit)))//Wl,Wm didn't match with r0l and r0m respectively
            {
                mismatch_x0 = 1;//error when x=0
                cout<<"fails at (X=0)"<<jBit;
                break;//breaks out of inner loop
            }
            if((X_bit == 1) & ((wl_bit != r1l_bit) | (wm_bit != r1m_bit)))//Wl,Wm didn't match with r1l and r1m respectively
            {
                mismatch_x1 = 1;//error when x=0
                cout<<"fails at (X = 1)"<<jBit;
                break;//breaks out of inner loop
            }
        }
        if((mismatch_x0 == 1) | (mismatch_x1 == 1)){
            test_flag = 0;
            break;//breaks out of the outer loop
        }
    }
    cout<<endl<<"OT test status========";
    if(test_flag == 0)
        cout<<"Test fails";
    else
        cout<<"Test passed";
}

void submod3_test(std::mt19937 &generator)
{
    uint64_t msb1,msb2;
    uint64_t lsb1,lsb2;
    uint64_t unpackedword1[64],unpackedword2[64], naivez3_bit[64], temp_bit[64];
    uint64_t outM,outL, subz3_bit[64];
    bool test_flag = true;
    uint64_t msb_bit,lsb_bit;
    generate_rand_Z3_packed_Word(msb1,lsb1,unpackedword1,generator);
    generate_rand_Z3_packed_Word(msb2,lsb2,unpackedword2,generator);
    /*cout<<endl<<"Value of lsb and msb generated for submod3 unit testing"<<endl;
    cout<<"msb: "<<msb1<<endl;
    cout<<"lsb: "<<lsb1<<endl;
    cout<<"msb: "<<msb2<<endl;
    cout<<"lsb: "<<lsb2<<endl;
    cout<<"First Number"<<endl;
    for(int i = 0; i < 64; i++)
    {
        cout<<unpackedword1[i];
    }
    cout<<endl<<"Second Number"<<endl;
    for(int i = 0; i < 64; i++)
    {
        cout<<unpackedword2[i];
    }*/
    //Call the submod3 function and perform the subtraction of two words
    subMod3(outM,outL,msb1,lsb1,msb2,lsb2);
    //cout<<endl<<"Output by Sub mod 3 function"<<endl;
    for(int i = 0; i < 64; i++)
    {

        msb_bit = (outM >> i) & 1;
        lsb_bit = (outL >> i) & 1;
        subz3_bit[i] = (msb_bit << 1 | lsb_bit);
        //cout<<subz3_bit[i];
    }
    //Manually/Naively compute the subtraction of the unpacked version of two words.
    //cout<<endl<<"Manual subtraction of two words unpacked"<<endl;
    for(int i = 0; i < 64; i++){
  //      if(unpackedword1[i] >= unpackedword2[i])
  //      {
            temp_bit[i] = (3+unpackedword1[i] - unpackedword2[i]);
            naivez3_bit[i] = temp_bit[i] % 3;
//        }
 //       else{
 //           temp_bit[i] = (unpackedword2[i] - unpackedword1[i]);
  //          naivez3_bit[i] = (3 -(temp_bit[i] % 3));
  //      }
        if(naivez3_bit[i] != subz3_bit[i])
        {
            test_flag = false;
            break;
        }
    }
    if(test_flag == true)
    {
        cout<<endl<<"Sub3 mod function unit test status - PASSED"<<endl;
    } else{
        cout<<endl<<"Sub3 mod function unit test status - FAILED"<<endl;
    }
}

void OTZ3_submodule_test(std::mt19937 &generator)
{
    //Variables for naive implementation
    uint64_t M0_packed[256], M1_packed[256], ra[256], rb[256], rx_unpacked[256], z[256], w[256], w_combined[256];
    //Variables for packed implementation
    uint64_t m0m[4],m0l[4], m1m[4],m1l[4], ram[4], ral[4], rbm[4], rbl[4], zm[4],zl[4],wm[4],wl[4],rx[4];

    //generate M0, M1, their msb's and lsb's in Z3
    generate_rand_Z3_packed_Vec_4(m0m,m0l,M0_packed,generator);//M0m and M0l are 4 words in Z3, M0 is 256 bits in z3
    generate_rand_Z3_packed_Vec_4(m1m,m1l,M1_packed,generator);//M1m and M1l are 4 words in Z3, M1 is 256 bits in z3

    setm0m1Z3Packed(m0m, m0l,  m1m, m1l);

    //generate ram, ral, rbl, rbm, rx(binary)
    generate_rand_Z3_packed_Vec_4(ram,ral,ra,generator);//ram and ral are 4 words in Z3, ra is 256 bits in z3
    generate_rand_Z3_packed_Vec_4(rbm,rbl,rb,generator);//rbm and rbl are 4 words in Z3, rb is 256 bits in z3

    //generate rx as a packed word, 4 packed words and unpack it for the naive use
    generate_rand_packed_vector_4(rx,generator);

    //compute z (word-packed) and w(word-packed)
    /*
     * There exists a function which can compute Wm and Wl (word-packed), but
     * it requires 4 words. Here we are dealing with just one word for the
     */
    for (int i = 0; i < 4; i++)
    {
        zm[i] = (ram[i] & rx[i]) ^ (rbm[i] & (~rx[i]));
        zl[i] = (ral[i] & rx[i]) ^ (rbl[i] & (~rx[i]));
    }

    //compute w (wordpacked)
    OTZ3_R_Part2Packed(rx,zm,zl,wm,wl);

    //rx is a unpacked binary vector
    for (int jWord = 0; jWord < 4; jWord++) //code from unpackOutput, but it generates 256 char output which couldn't be changed in dmweakPRFpacked
    {
        for (int iBit = 0; iBit < 64; iBit++) {
            int i = iBit+jWord*wLen;
            rx_unpacked[i] = ((rx[jWord] >> iBit) & 1) ;
            z[i] = ((rx_unpacked[i] * ra[i]) + ((1-rx_unpacked[i]) * rb[i]));
            w[i] = ((3 + (rx_unpacked[i] * M1_packed[i]) + ((1-rx_unpacked[i]) * M0_packed[i])) - z[i]) % 3;
        }
    }

    uint64_t zm_bits[256];
    uint64_t zl_bits[256];
    uint64_t z_combined[256];

    //combine value of wm and wl as 4 word packed w_pack
    for(int jWord = 0; jWord < 4; jWord++)
    {
        uint64_t wm_word = wm[jWord];
        uint64_t wl_word = wl[jWord];
        uint64_t zm_word = zm[jWord];
        uint64_t zl_word = zl[jWord];

        for(int iBit = 0; iBit < 64; iBit++)
        {
            int i = iBit+jWord*wLen;
            uint64_t wm_bit = (wm_word>>iBit) & 1;
            uint64_t wl_bit = (wl_word>>iBit) & 1;
            w_combined[i] = (wm_bit<<1) | wl_bit;
            zm_bits[i] = (zm_word>>iBit) & 1;
            zl_bits[i] = (zl_word>>iBit) & 1;
            z_combined[i] = (zm_bits[i]<<1) | zl_bits[i];
        }
    }

    //Compare naive w with combined Z3 value w_pack
    bool OT_mod_test_flag = true; //set the test flag
    for(int cnt = 0; cnt < 256; cnt++)
    {
        if(w[cnt] != w_combined[cnt])
        {
            OT_mod_test_flag = false;
            break;
        }
    }
    if(OT_mod_test_flag == false)
        cout<<endl<<"OT module test status =====> FAILED"<<endl;
    else
        cout<<endl<<"OT module test status =====> PASSED"<<endl;

}