#include <iostream>
#include <random>
#include <bitset>
#include <cmath>
#include <chrono> 
#include <ctime> 
#include <cstdlib>


//unsigned long int z_final[4];//to store the final product values

using namespace std;

//This function will create the input vector randomly and store that value into the array named input.
void generate_input(bool input[256], std::mt19937 &generator)
{
	srand(time(NULL));
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
	generate_rand_key(key, generator);
    generate_input(input,generator);

    chrono::time_point<std::chrono::system_clock> start, end; 
    unsigned long int z_final[4];
    
    start = chrono::system_clock::now(); 

    for(int i=0;i<1000000;i++){
    
        compute(key,input, z_final);
    }

	

    end = chrono::system_clock::now();
    chrono::duration<double> elapsed_seconds = end - start; 
    time_t end_time = chrono::system_clock::to_time_t(end); 
  
    cout<<endl<<"Value of z is "<< z_final << "\n";

    cout << "Finished at " << ctime(&end_time) 
              << "elapsed time for 1M runs:  " << elapsed_seconds.count() << "  s\n";
              
              



	return 0;
}
