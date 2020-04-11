#include<iostream>
#include<bitset>

//std::bitset<8> input;
int len, row, col;
int secret_key[8][8];
int input[8];
int toep_values[15];
int pack_key[8][8];
//int temp_key[4];
int temp_input[4];

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
	for(int i = 0; i < 8; i++)
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

void calculate()
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
	

}

void pack()//packing the bit in secret key.
{
	int temp;
	int k[];
	int pack_size = 4;
	for(int col = 0; col < len; col++)
	{
		for(int row = 0;row < len/pack_size; row=row+pack_size)
		{
			int temp = secret_key[row][col]*1000 + secret_key[row+1][col]*100 + secret_key[row+2][col]*10 + secret_key[row+3][col]
		}
	}




	for(int row = 0; row < len; row++)
	{
		
	}
}


int main()
{
	std::cout<<"Enter the value of security parameter(length of your input)"<<std::endl;
	std::cin>>len;
	generate_input();
	generate_toeplitz();
	display_values();
	compute();
	//calculate();
	return 0;
}
