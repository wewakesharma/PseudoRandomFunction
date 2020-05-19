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
void generate_z3_input(int input[256], std::mt19937 &generator)
{
    //srand(time(NULL));
    for(int i = 0; i < 256; i++)
    {
        input[i] = generator() & 1;
    }
    //Output the input generated
    cout<<endl<<"Input generated"<<endl;
    for(int i = 0; i < 256; i++){
        cout<<input[i];
    }
}

//81X128 sized randomly generated matrix in Z3 converted to 81 X 256 in Z2.
void generate_rand_matrix(int rand_matrix[81][256], std::mt19937 &generator)
{
    int temp;
    for (int i = 0; i < 81; i++)
    {
        for (int j = 0; j < 128; j++)
        {
            std::bitset<2> x(generator() % 3);
            rand_matrix[i][2*j] = x[1];
            rand_matrix[i][2*j+1] = x[0];
        }
    }
    cout<<endl<<"Displaying contents of random matrix"<<endl;
    for (int i = 0; i < 81; i++)
    {
        for (int j = 0; j < 256; j++)
        {
            cout<<rand_matrix[i][j];
            cout<<' ';
        }
        cout<<endl;
    }
}


/*bitCount(): counts the number of '1-bit' in z_final arrays and sends it back to compute() to calculate mod-3 value.
z_final is converted into decimal from unsigned long int and bit_counter stores the number of 1 in it.*/



/*compute(): takes 2-d array key, 1-d array input and stores their product in z_final.
Calls the procedure bitCount() which sends the total number of bits and it computes its mod-3 value
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
}*/




//Step 1: Generate random input and store it in input array.
//Step 2: Generate 1024 uint64 numbers and store it in 4X256 size.
//Step 3: compute() function will calculate the value of zi's
int main()
{
    uint64_t key[4][256];
    int rand_matrix[81][256];
    int input[256];
    unsigned seed = 7;            // std::chrono::system_clock::now().time_since_epoch().count();
    int mod3_value;

    std::mt19937 generator(seed); // mt19937 is a standard mersenne_twister_engine
    //generate_rand_key(key, generator);
    generate_z3_input(input,generator);
    generate_rand_matrix(rand_matrix,generator);


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
