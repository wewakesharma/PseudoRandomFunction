#include <iostream>
#include <random>
#include <bitset>
#include <cmath>
#include <chrono> 
#include <ctime> 
#include <cstdlib>


//To-DO list
//1. Generate a 81 X 256 size matrix in Z3.
//2. Multiply this matrix with a 256 bit matrix we get from earlier phase(question is, Is this in Z2 or Z3?)


using namespace std;

//For now, consider this as 256 bit we received at the end of phase 2 when we unpacked 4 words, each of 64 bit size.
void generate_input(std::bitset<256> input,std::bitset<256> key, std::bitset<256> out, std::mt19937 &generator)
{
    //int total_count=0;
    for(int i = 0; i < 256; i++)
    {
        input[i] = generator() & 1;
    }
    //===================
    for(int cnt=0;cnt<81;cnt++)
    {
        key = 0;
        for (int i = 0; i < 128; i++)
        {
            std::bitset<2> x(generator() % 3);
            key[2*i] = x[1];
            key[2*i+1] = x[0];
        }
        out = (key & input);
        out.count()%3;
        //cout<<endl<<out.count()%3;
    }
    
}




//Step 1: Generate random input and store it in input array.
//Step 2: Generate 1024 uint64 numbers and store it in 4X256 size.
//Step 3: compute() function will calculate the value of zi's
int main()
{
    std::bitset<256> input;
    std::bitset<256> key;
    std::bitset<256> out;
    unsigned seed = 7;            // std::chrono::system_clock::now().time_since_epoch().count();

    std::mt19937 generator(seed); // mt19937 is a standard mersenne_twister_engine
    chrono::time_point<std::chrono::system_clock> start, end; 
    start = chrono::system_clock::now();
    generate_input(input,key,out,generator);
    end = chrono::system_clock::now();
    chrono::duration<double> elapsed_seconds = end - start; 
    time_t end_time = chrono::system_clock::to_time_t(end); 
    cout << "Finished at " << ctime(&end_time) 
              << "elapsed time to generate 81 bits of Z3:  " << elapsed_seconds.count() << "  s\n";
    

    return 0;
}
