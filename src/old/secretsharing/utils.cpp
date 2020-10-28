#include <iostream>
#include <random>
#include <bitset>
#include <cmath>
#include <chrono> 
#include <ctime> 
#include <cstdlib>


//unsigned long int z_final[4];//to store the final product values

using namespace std;


void generate_rand_sqMat_256(uint64_t mat[256][256], std::mt19937 &generator)
{

    for(int j = 0; j < 256; j++)
        for(int i = 0; i < 256; i++)
            {
            //Call the generator that generate a 64-but word each time
                mat[i][j] = generator() & 1;
            }
}

//
/*
 * Generate 4 words of random data into the input array.
 */
void generate_rand_vector_256(uint64_t vec[256], std::mt19937 &generator)
{

    //srand(time(NULL));
    for(int i = 0; i < 256; i++)
    {
        //Call the generator that generate a 64-but word each time
        vec[i] = generator() & 1;
    }
}




/*
 * generate a random packed vector
 */




