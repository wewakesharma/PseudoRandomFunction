#include <iostream>
#include <random>
#include <bitset>
#include <cmath>
#include <chrono> 
#include <ctime> 
#include <cstdlib>

#include "dmweakPRF.h"

//int wLen = 64;


//unsigned long int z_final[4];//to store the final product values

using namespace std;




//Step 1: Generate random input and store it in input array.
//Step 2: Generate 1024 uint64 numbers and store it in 4X256 size.
//Step 3: compute() function will calculate the value of zi's
int main(int argc,char* argv[] ) {
    uint64_t key[4][256];
    //randMat1 holds the LSB's, randMat2 holds the MSB's of the randomization matrix
    uint64_t randMat1[2][256], randMat2[2][256];
    uint64_t randMatZ3[81][256]; //randMatZ3 holds the Z3 elements
    uint64_t input[4];
    uint64_t outM[2];
    uint64_t outL[2];
    uint64_t output_p1[4];
    uint64_t output_p3[84];

    int stepsToRun;

    if (argc > 1) {
        char *p;
        stepsToRun = strtol(argv[1], &p, 10);
    }
    else
        stepsToRun=3;

  //  cout << "Please enter the number of steps to run    : ";
  //  cin >> stepsToRun;
    cout << "The number of phases to run you entered is " << stepsToRun << endl;

    unsigned seed = 7;    // std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed); // mt19937 is a standard mersenne_twister_engine
	generate_rand_key(key, generator);

	//we generate two random matrices, one holds the first bit and one the second bit
	generate_rand_matrix(randMat1, randMat2, randMatZ3, generator);
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

	return 0;
}
