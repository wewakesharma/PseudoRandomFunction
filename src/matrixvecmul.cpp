#include<iostream>
#include<bitset>
#include<cmath>

//std::bitset<8> input;
int len, row, col; //len is the value of security parameter, also the dimension of matrix. row and col are also same, they can be removed
int secret_key[256][256];
int input[256];
int toep_values[512];
int packed_key[25];
int temp_input[4];
int pack_size = 4;

using namespace std;

//This function will create the input vector randomly and store that value into the array named input.
void generate_input()
{
	srand(time(NULL));
    for(int i = 0; i < len; i++)
    {
        input[i] = rand() & 1;
    }
}


void generate_toeplitz()
{
	//generating random number for toeplitz values. Toeplitz will have two parts base and append. base will be decreasing and append ptr
	//will increase
	srand(time(NULL));
	//total number to be generated for a toeplitz matrix is (2*n - 1)
	for(int i = 0; i < (2*len-1); i++)
	{
		toep_values[i] = rand() & 1;
	}

	int append = len;
	int base = len;
	int k = 0;
	//Filling up the top part of toeplitz matrix
	
	for(int i = 0; i < len; i++)
	{
		//Filling up the upper triangular matrix
		int k = 0;
		for(int j = i; j < len;j++)
		{
			secret_key[i][j] = toep_values[k];
			k++;
		}

		for(int j = 0; j < i+1; j++)
		{
			secret_key[i+1][j] = toep_values[(base-j)];
		}
		base++;
	}	
}

void display_values()
{
	cout<<"The input generated is "<<std::endl;
	for(int i = 0; i < len; i++)
	{
		std::cout<<input[i];
	}
	std::cout<<endl;
	cout<<"The toeplitz key generated is "<<std::endl;
	for(int i = 0; i < (2*len-1); i++)
	{
		std::cout<<toep_values[i];
	}
	std::cout<<endl;
	cout<<"The toeplitz secret key matrix generated is "<<std::endl;
	for(int i = 0; i < len; i++)
	{
		for(int j = 0; j < len; j++)
		{
			std::cout<<secret_key[i][j]<<"\t";
		}
		std::cout<<"\n";
	}
	std::cout<<endl;
}

/*void calculate()
{
	//int temp_input[4];
	int temp_key[4];

	//Calculating Z1.


	//Calculating Z1
	for(int col = 0; col < len/2; col++)
	{
		if(input[(len/2) - col - 1] == 1)
		{
			for(int row = 0; row < len/2; row++)
			{
				z1[row] = z1[row] + secret_key[row][col];
			}
		}
	}
	
	//Calculating Z2
	for(int col = len/2; col < len; col++)
	{
		if(input[(len) - col - 1] == 1)
		{
			for(int row = len/2; row < len; row++)
			{
				z1[row] = z1[row] + secret_key[row][col];
			}
		}
	}

	//Displaying the values of temp key
	for(int i = 0; i < len/2; i++)
	{
		cout<<temp_key[i];
	}
}

void compute()
{
	//int pack_size = 4;
	int temp;
	

}*/

void pack()//packing the bit in secret key.
{	
	int i = 0;
	for(int col = 0; col < len; col++)
	{
		for(int row = 0;row <= pack_size; row=row+pack_size)
		{
			int temp = secret_key[row][col]*pow(2,3) + secret_key[row+1][col]*pow(2,2) + secret_key[row+2][col]*pow(2,1) + secret_key[row+3][col];
			packed_key[i] = temp;
			i++;
		}
		row = 0;
	}
}

void compute()
{
	int z1 = 0;
	int z2 = 0;
	int temp;
	int d;
	//compute z1
	for(int i = 0; i < len/pack_size; i++)
	{
		for(int j = 0; j < pack_size; j++)
		{
			d = 4*i;
			z1 = ((input[d+j] * packed_key[8*i + 6]) ^ 
			(input[d+j] * packed_key[8*i + 4]) ^
				(input[d+j] * packed_key[8*i + 2]) ^
					(input[d+j] * packed_key[8*i]));

		}
	}
	cout<<"Z1 is "<<z1<<endl;

	//compute z2
	for(int i = 0; i < len/pack_size; i++)
	{
		for(int j = 0; j < pack_size; j++)
		{
			d = 4*i;
			z2 = ((input[d+j] * packed_key[8*i + 7]) ^ 
			(input[d+j] * packed_key[8*i + 5]) ^
				(input[d+j] * packed_key[8*i + 3]) ^
					(input[d+j] * packed_key[8*i + 1]));

		}
	}
	cout<<"Z2 is "<<z2<<endl;

}

//Step 1: Generate random input and store it in input array.
//Step 2: Generate 2n-1 bits and store it toep_values array. 
//Step 3: Create a nxn toeplitz matrix. This step can be bypassed since we no longer need the bits in matrix but packed in a word.
//Step 4: pack() function will pack four bits into one.
//Step 5: compute() function will calculate the value of z1 and z2.
int main()
{
	std::cout<<"Enter the value of security parameter(length of your input)"<<std::endl;
	std::cin>>len;
	generate_input();
	generate_toeplitz();
	display_values();
	//pack();
	//compute();
	return 0;
}
