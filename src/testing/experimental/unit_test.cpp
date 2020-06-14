//main file that includes header file

#include<iostream>
#include <random>
#include <bitset>
#include <cmath>
#include <chrono> 
#include <ctime> 
#include <cstdlib>
#include "weak23prf.h"

using namespace std;

int main(int argc,char* argv[] ) 
{
	//===========General===============
    int choice; //choice for choosing the type of testing required
    //========Phase 1====================
    uint64_t key[4][256];
    uint64_t input[4];
    uint64_t output[4];
    int unpacked_input[256];
    uint64_t out[256];  //output of phase 1(naive)
    char p2output[256];
    //========== Phase 2/3==================
    uint64_t randMat1[2][256], randMat2[2][256];
    uint64_t outM[2];
    uint64_t outL[2];
    uint64_t z3_mat[81][256];
    uint64_t phase3_out[81];
    uint64_t p3_out[81];//packed version output

    unsigned seed = 7;    // std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);
    //Input generation
    generate_input(input, generator, 4); //passes the address of input, generator and size of input array to the function in head.cpp
    generate_rand_key(key, generator);
	generate_rand_matrix(randMat1, randMat2, generator);
	cout<<endl<<"Random input generated ==========>   O.K."<<endl;

	//Phase 1
    cout<<"Initializing unit testing for phase 1 ==========>   O.K."<<endl;
    compute(key,input, output); // matrix-vector multiply mod 2
    //unpackOutput(output,p2output); // useless operation that should not be here
    mat_vec_mult(input, key, out);
    phase1_test(output,out);//compares the output of two different approaches
    cout<<"==========Phase 1 testing complete=========="<<endl<<endl;
	return 0;
}