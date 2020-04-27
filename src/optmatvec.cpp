#include <iostream>
#include <random>
#include <bitset>
#include <cmath>
#include <chrono> 
#include <ctime> 
#include <cstdlib>

unsigned long int n;
int total_bit_count;
//unsigned long int z_final[4];//to store the final product values

using namespace std;

//This function will create the input vector randomly and store that value into the array named input.
void generate_input(bool input[256], std::mt19937 &generator)
{
    //srand(time(NULL));
    for(int i = 0; i < 256; i++)
    {
        input[i] = generator() & 1;
    }
}

//This function will create the input vector randomly and store that value into the array named input.
void generate_rand_key(uint64_t key[4][256], std::mt19937 &generator)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 256; j++)
        {
            key[i][j] = generator();
        }
    }
}

int bitCount(unsigned long int z_final[4]) 
{
    int bit_counter = 0;
    for(int z_count = 0; z_count < 4; z_count++)
    {
        n = z_final[z_count];
        //compute total number of 1-bits in z_final
        while(n) 
        {
            bit_counter += n % 2;
            n >>= 1;
        }
        //perform mod-3 on it
    }
    return bit_counter;
 }

void compute(uint64_t key[4][256], bool input[256], unsigned long int z_final[4])
{

    for(int i=0; i < 4; i++)
    {
        z_final[i] = 0;
        for(int j = 0; j < 256; j++)
        {
            if(input[j]==1)
            {
                z_final[i] = z_final[i] ^ (key[i][j]);
            }   
        }
    
    }
    total_bit_count = bitCount(z_final);
    cout<<total_bit_count%3<<endl;
}




//Step 1: Generate random input and store it in input array.
//Step 2: Generate 1024 uint64 numbers and store it in 4X256 size.
//Step 3: compute() function will calculate the value of zi's
int main()
{
    uint64_t key[4][256];
    bool input[256];
    unsigned seed = 7;            // std::chrono::system_clock::now().time_since_epoch().count();

    std::mt19937 generator(seed); // mt19937 is a standard mersenne_twister_engine
    //generate_rand_key(key, generator);
    //generate_input(input,generator);

    chrono::time_point<std::chrono::system_clock> start, end; 
    unsigned long int z_final[4];
    
    start = chrono::system_clock::now(); 

    //for(int i=0;i<1000000;i++)
    //{
    for(int i=0;i<100;i++)
    {
        generate_input(input,generator);
        for(int j=0;j<1000;j++)
        {
            generate_rand_key(key, generator);
            compute(key,input, z_final);
        }   
    }

    

    end = chrono::system_clock::now();
    chrono::duration<double> elapsed_seconds = end - start; 
    time_t end_time = chrono::system_clock::to_time_t(end); 
  
    //cout<<endl<<"Value of z is "<< z_final << "\n";

    cout << "Finished at " << ctime(&end_time) 
              << "elapsed time for 1M runs:  " << elapsed_seconds.count() << "  s\n";

    return 0;
}
