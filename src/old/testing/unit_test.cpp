//main file that includes header file

#include<iostream>
#include <random>
#include <bitset>
#include <cmath>
#include <chrono> 
#include <ctime> 
#include <cstdlib>
#include "weakprf.h"

using namespace std;

int main(int argc,char* argv[] ) 
{
	//===========General===============
    int choice; //choice for choosing the type of testing required
    //========Phase 1====================
    uint64_t key[4][256];//generated using generate_rand_key
    uint64_t input[4]; //generate_input
    uint64_t output[4]; //packed version output of phase 1
    int unpacked_input[256];
    uint64_t naive_out_p1[256];  //output of phase 1(naive)- testing
    char p2output[256];
    //========== Phase 2/3==================
    uint64_t randMat1[2][256], randMat2[2][256];
    uint64_t randMatZ3[81][256]; //randMatZ3 holds the Z3 elements
    uint64_t outM[2];
    uint64_t outL[2];
    uint64_t z3_mat[81][256];
    uint64_t naive_out_p3[81];
    uint64_t p3_out[81];//packed version output of phase 3

    //PRF alternate method
    uint64_t output_p3_alter1[84];//84 bit output of phase 3 using alternate method InnerProdMul
    uint64_t output_p3_alter2[84];//84 bit output of phase 3 using alternate method InnerProdMul2

    unsigned seed = 7;    // std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);
    //Input generation
    generate_input(input, generator); //passes the address of input, generator and size of input array to the function in head.cpp
    generate_rand_key(key, generator);
	generate_rand_matrix(randMat1, randMat2, randMatZ3, generator);
	cout<<endl<<"Random input generated ==========>   O.K."<<endl;

	//Phase 1
    cout<<endl<<"Initializing unit testing for phase 1 ==========>   O.K."<<endl;
    compute(key,input, output); // matrix-vector multiply mod 2
    //unpackOutput(output,p2output); // useless operation that should not be here
    mat_vec_mult(input, key, naive_out_p1);
    phase1_test(output,naive_out_p1);//compares the output of two different approaches
    cout<<"==========Phase 1 testing complete=========="<<endl<<endl;

    //Phase 3
    cout<<"Initializing unit testing for phase 3 ==========>   O.K."<<endl;
    multMod3(outM, outL, randMat1, randMat2, output, p3_out); // matrix-vector multiply mod 3
    phase3_naive(naive_out_p1, randMatZ3, naive_out_p3);
    phase3_test(naive_out_p3, p3_out);
    cout<<"==========Phase 3 testing complete=========="<<endl;

    //PRF alternate method
    //InnerProdMul(output_p3_alter1, randMatZ3, output);
    
    InnerProdMul(output_p3_alter1, randMatZ3, output, naive_out_p3);
    InnerProdMul_test(output_p3_alter1,naive_out_p3);
    InnerProdMul2(output_p3_alter2, randMatZ3, output, naive_out_p3);
    InnerProdMul2_test(output_p3_alter1,naive_out_p3);
return 0;
}