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

void generate_input_naive(int input[256], std::mt19937 &generator)
{
    //srand(time(NULL));
    for(int i = 0; i < 16; i++)
    {
        input[i] = generator() &1;
    }
}

//This function will create the input vector randomly and store that value into the array named input.
void generate_key_naive(int key[16][16], std::mt19937 &generator)
{
    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 16; j++)
        {
            key[i][j] = generator() & 1;
        }
    }
}



uint64_t bit_multiply(uint64_t a, uint64_t b) //russian peasant method
{ 
    int res = 0;  // initialize result 
  
    // While second number doesn't become 1 
    //we know the number of bits in integer, loop can run for fixed amount of number.
    while (b > 0) 
    { 
         // If second number becomes odd, add the first number to result 
         if (b & 1) 
             res = res + a; 
  
         // Double the first number and halve the second number 
         a = a << 1; 
         b = b >> 1; 
     } 
     return res
} 
int main()
{
    uint64_t key;
    uint64_t input;
    uint64_t result;
    unsigned seed = 5;            // std::chrono::system_clock::now().time_since_epoch().count();
    

    std::mt19937 generator(seed); // mt19937 is a standard mersenne_twister_engine

    input = generator()&0xffff;
    cout<<input<<endl;
    std::bitset<32> input_bits(input);
    cout<<input_bits<<endl;
    //cout<<(x<<2)<<endl;
    key = generator()&0xffff;
    cout<<key<<endl;
    std::bitset<32> key_bits(key);
    cout<<key_bits<<endl;
    result = bit_multiply(input, key);
    cout<<"Bitwise multiplication of key and input is "<<result<<endl;
    //std::bitset<64> res_bit(result);
    //cout<<res_bit<<endl;
    //mod3_value = compute(key,input, z_final);  
    //cout<<endl<<"Value of z is "<< z_final << "\n";
    //cout<<endl<<"Value of the PRF is "<< mod3_value << "\n";
    return 0;
}
