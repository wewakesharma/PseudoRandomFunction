//Unit Testing
/*
Step 1: Take an key of size 16 x 16 bit secret key
Step 2: Take a 16 bit input
Step 3: pack 16 X 16 bit secret key into 4 X 16 sized integer array
Step 4: multliply 4 X 16 key with 16 bit input( use a bit operator), resultant is a vector of size 4. Keep it packed
Ste 5: Take the vector of size 4 and multiply it with 81 X 4 sized matrix. 
Step 5: resutant is a 81 bit vector.
*/
#include <iostream>
#include <random>
#include <bitset>
#include <cmath>
#include <chrono> 
#include <ctime> 
#include <cstdlib>


using namespace std;

void generate_input(bool input[4], std::mt19937 &generator)
{
    //srand(time(NULL));
    for(int i = 0; i < 4; i++)
    {
        input[i] = generator();
    }
}

void generate_rand_key(uint64_t key[4][4], std::mt19937 &generator)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            key[i][j] = generator();
        }
    }
}
int main()
{
    uint64_t key[4][16];
    bool input[256];
    unsigned seed = 7;            // std::chrono::system_clock::now().time_since_epoch().count();
    int mod3_value;

    std::mt19937 generator(seed); // mt19937 is a standard mersenne_twister_engine
    //generate_rand_key(key, generator);
    //generate_input(input,generator);

    unsigned long int z_final[4];
    generate_rand_key(key, generator);
    generate_input(input,generator);
    mod3_value = compute(key,input, z_final);  
    cout<<endl<<"Value of z is "<< z_final << "\n";
    cout<<endl<<"Value of the PRF is "<< mod3_value << "\n";
    return 0;
}
