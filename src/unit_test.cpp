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

uint64_t bit_multiply(uint64_t a, uint64_t b) 
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
     return res; 
} 
int main()
{
    uint64_t key;
    uint64_t input;
    uint64_t result;
    unsigned seed = 7;            // std::chrono::system_clock::now().time_since_epoch().count();
    

    std::mt19937 generator(seed); // mt19937 is a standard mersenne_twister_engine
    //generate_rand_key(key, generator);
    //generate_input(input,generator);

    //unsigned long int z_final[4];
    input = generator()&0xff;
    cout<<input<<endl;
    std::bitset<32> x(input);
    cout<<x<<endl;
    //cout<<(x<<2)<<endl;
    key = generator()&0xff;
    cout<<key<<endl;
    result = bit_multiply(input, key);
    cout<<"Bitwise multiplication of key and input is "<<result<<endl;
    std::bitset<64> res_bit(result);
    cout<<res_bit<<endl;
    //mod3_value = compute(key,input, z_final);  
    //cout<<endl<<"Value of z is "<< z_final << "\n";
    //cout<<endl<<"Value of the PRF is "<< mod3_value << "\n";
    return 0;
}
