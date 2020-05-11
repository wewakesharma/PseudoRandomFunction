#include <iostream>
#include <random>
#include <bitset>
#include <cmath>
#include <chrono> 
#include <ctime> 
#include <cstdlib>

//To-DO list
//1. Generate a 81 X 128 size matrix in Z3. Later it can be expanded to 81 X 256 matrix in Z2. 
//2. Multiply this matrix with a 256 bit matrix we get from earlier phase(question is, Is this in Z2 or Z3?)




using namespace std;

//For now, consider this as 256 bit we received at the end of phase 2 when we unpacked 4 words, each of 64 bit size.
void generate_input(int input[256], std::mt19937 &generator)
{
    //srand(time(NULL));
    for(int i = 0; i < 256; i++)
    {
        input[i] = generator()%3;
    }
    //Output the input generated
    cout<<endl<<"Input generated"<<endl;
    for(int i = 0; i < 256; i++){
        cout<<input[i];
    }
}


//Generate toeplitz matrix of size 81X256. Total bits needs to be generated is 256+80 = 336 bits.
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

//Output of matrix vector multiplication will be multiplied by 81X256 sized randomly generated matrix in Z3.
void generate_rand_matrix(uint64_t rand_matrix[81][256], std::mt19937 &generator)
{
    for (int i = 0; i < 81; i++)
    {
        for (int j = 0; j < 256; j++)
        {
            rand_matrix[i][j] = generator();
        }
    }
}


/*bitCount(): counts the number of '1-bit' in z_final arrays and sends it back to compute() to calculate mod-3 value.
z_final is converted into decimal from unsigned long int and bit_counter stores the number of 1 in it.*/

int bitCount(unsigned long int z_final[4]) 
{
	unsigned long int n;
    int bit_counter = 0;
    for(int z_count = 0; z_count < 4; z_count++)
    {
        n = z_final[z_count];
        while(n) 
        {
            bit_counter += n & 1;
            n >>= 1;
        }
    }
    return bit_counter;
 }

/*compute(): takes 2-d array key, 1-d array input and stores their product in z_final.
Calls the procedure bitCount() which sends the total number of bits and it computes its mod-3 value*/
int compute(uint64_t key[4][256], bool input[256], unsigned long int z_final[4])
{
    //Declare and define the array that will store 81 bit PRF output value
    //Loop the contents 81 times to generate 81 Z3 elements.
    //send the array back to main function or create a separate function which has already generated 81X256 bits random matrix in Z3
    //multiply them
	int total_bit_count = 0;
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
    return (total_bit_count%3);
}




//Step 1: Generate random input and store it in input array.
//Step 2: Generate 1024 uint64 numbers and store it in 4X256 size.
//Step 3: compute() function will calculate the value of zi's
int main()
{
    uint64_t key[4][256];
    uint64_t rand_matrix[4][256];
    int input[256];
    unsigned seed = 7;            // std::chrono::system_clock::now().time_since_epoch().count();
    int mod3_value;

    std::mt19937 generator(seed); // mt19937 is a standard mersenne_twister_engine
    //generate_rand_key(key, generator);
    generate_input(input,generator);
    //generate_rand_matrix(rand_matrix,generator);


    //chrono::time_point<std::chrono::system_clock> start, end; 
    unsigned long int z_final[4];
    
    /*start = chrono::system_clock::now(); 

    for(int i=0;i<1000;i++)//This loop took 5.35 seconds{
        generate_rand_key(key, generator);
        for(int j=0;j<1000;j++){
            generate_input(input,generator);
            mod3_value = compute(key,input, z_final);
        }   
    }
    end = chrono::system_clock::now();
    chrono::duration<double> elapsed_seconds = end - start; 
    time_t end_time = chrono::system_clock::to_time_t(end); 
  
    cout<<endl<<"Value of z is "<< z_final << "\n";
   cout<<endl<<"Value of the PRF is "<< mod3_value << "\n";

    cout << "Finished at " << ctime(&end_time) 
              << "elapsed time for 1M runs:  " << elapsed_seconds.count() << "  s\n";*/

    return 0;
}