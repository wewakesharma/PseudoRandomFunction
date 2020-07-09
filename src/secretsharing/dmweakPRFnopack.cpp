#include <iostream>
#include <random>
#include <bitset>
#include <cmath>
#include <chrono> 
#include <ctime> 
#include <cstdlib>

#include "dmweakPRF.h"
#include "dmweakPRFnoPack.h"

//unsigned long int z_final[4];//to store the final product values

using namespace std;


/*
 * compute the multiplication of the key with the input
 * Each word holds 64 packed binary bits
 * //the non-packed version - To write
 */
void VecMatMultnotPack(uint64_t key[256][256], uint64_t input[256], uint64_t z_final[256])
{
    for(int i=0; i < 256; i++) {
        z_final[i] = 0; // initialize the accumulator to zero
    }

    for (int k = 0; k < 256; k++)
        for(int jCol = 0; jCol < 256; jCol++)
		{
                uint64_t z = key[k][jCol] & input[jCol] ;
                z_final[k] ^= z;

		}
}

void VecMatMultnotPack2(uint64_t key[128][256], uint64_t input[256], uint64_t z_final[256])
{
    for(int i=0; i < 128; i++) {
        z_final[i] = 0; // initialize the accumulator to zero
    }

    for (int k = 0; k < 256; k++)
        for(int jCol = 0; jCol < 128; jCol++)
        {
            uint64_t z = key[k][jCol] & input[jCol] ;
            z_final[k] ^= z;

        }
}

