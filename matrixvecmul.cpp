#include<iostream>
#include<bitset>

//std::bitset<8> input;
int len, row, col;
int secret_key[8][8];
int input[8];
int toep_values[15];

using namespace std;

void generate_input()
{
	srand(time(NULL));
    for(int i = 0; i < 8; i++)
    {
        input[i] = rand() & 1;
    }
}

void generate_toeplitz(){
	//generating random number for toeplitz values. Toeplitz will have two parts base and append. base will be decreasing and append ptr
	//will increase
	srand(time(NULL));
	for(int i = 0; i < (2*len-1); i++)
	{
		toep_values[i] = rand() & 1;
	}

	int append = len;
	int base = len;

	//Filling up the top part of toeplitz matrix
	for(int i = 0; i < len; i++)
	{

		for(int j = 0; j < i; j++)
		{
			for(int cnt = base; cnt > len; cnt--)
			{
				secret_key[i][j] = toep_values[cnt];	
			}
			base++;
		}

		int k = 0;
		for(int j = i; j < len;j++)
		{
			secret_key[i][j] = toep_values[k];
			k++;
		}
	}
	//Filling lower part of toeplitz matrix
	// int cnt = base;
	// for(int i = 1; i < len; i++)
	// {
	// 	for(int j = 0; j < i; j++)
	// 	{
	// 		for(cnt = base; cnt > len; cnt--)
	// 		{
	// 			secret_key[i][j] = toep_values[cnt];	
	// 		}
	// 		base++;
	// 	}
		
	// }
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


int main()
{
	std::cout<<"Enter the value of security parameter(length of your input)"<<std::endl;
	std::cin>>len;
	generate_input();
	generate_toeplitz();
	display_values();
	return 0;
}
