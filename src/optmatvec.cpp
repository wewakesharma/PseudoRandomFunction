#include <iostream>
#include <random>
#include <bitset>
#include <cmath>
#include <chrono> 
#include <ctime> 
#include <cstdlib>

//std::bitset<8> input;
int len=256; //len is the value of security parameter, also the dimension of matrix
int row, col;
int secret_key[256][256];

int toep_values[512];//Contains bits 
int packed_key[256];
int temp_input[4];
int pack_size = 64;//word size
unsigned long int z_final[4];//to store the final product values

unsigned long int packed_int = 0;

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

void compute(uint64_t key[4][256], bool input[256])
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
		cout<<endl<<"Value of z"<<i<<" is "<<z_final[i];
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

	//Key value display
	std::cout<<"Displaying the value of keys";
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 256; j++)
        {
            std::cout<<key[i][j];
            std::cout<<"\t";
        }
        std::cout<<"\n\n";
    }

    //input value display
    std::cout<<"displaying the value of input";
    generate_input(input,generator);
    for(int i=0;i<len;i++)
    {
    	cout<<input[i];
    }
    compute(key,input);
	/*chrono::time_point<std::chrono::system_clock> start, end; 
    start = chrono::system_clock::now(); 
    for(int i=0;i<10;i++)
    {
    	generate_input();
    	for(int j=0;j<100;j++)
    	{
    		generate_toeplitz();
			//display_values();
			pack();
			compute();
    	}
    }
	end = chrono::system_clock::now();
    chrono::duration<double> elapsed_seconds = end - start; 
    time_t end_time = chrono::system_clock::to_time_t(end); 
  
    cout << "Finished at " << ctime(&end_time) 
              << "elapsed time: " << elapsed_seconds.count() << "s\n";
              */
	return 0;
}
