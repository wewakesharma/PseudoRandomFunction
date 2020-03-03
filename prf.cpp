//
//  main.cpp
//  PRF
//
//  Created by Vivek Sharma on 12/25/19.
//  Copyright © 2019 vivek. All rights reserved.
//

//Mod-2/Mod-3 implementation of Pseudorandom Function
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include<typeinfo>
//#include<stdlib>
using namespace std;

int** secret_key;
string input_str;
int input[256];
int lambda;
int m = lambda;
int n = lambda;
int out_val;
int total_bits = 1;
void generateInput()
{
	srand(time(NULL));
    for(int i = 0; i < lambda; i++)
    {
        input[i] = rand() & 1;
    }
}


//=============================================================================
//Steps:
//1.Generate a random number. Get its binary representation
 //2.lay it out in matrix representation according to value of m and n

//Functions used:
//1.main function: calls every other function, start of the program
//2.generateRandom: generates a random number
//3.bitGeneration: convert random number to decimal representation
//2. matrixCreation: convert the random number into its binary representation.

//=============FUNCTION TO GENERATE RANDOM DIGITS=========
unsigned long generaterandom()//It will generate random numbers
{
    srand(time(NULL));
    unsigned long rand_num = rand();
    cout<<"Original Random Number is "<<rand_num<<endl;
    rand_num = rand_num;
    return rand_num;
}

//=========THIS FUNCTION CONVERTS RANDOM DECIMAL TO RANDOM BINARY=======
int* randomBinary(int bit_len)
{
    int call_count = bit_len/30 + 1;
    int* rand_bit;
    rand_bit = new int [bit_len];
    int i = 0;
    for(int cnt = 0; cnt<call_count;cnt++)
    {
        unsigned long num = generaterandom();
        cout<<"The value of random number is "<<num<<endl;
        while (num > 0)
        {
            rand_bit[i] = num % 2;
            num = num / 2;
            i++;
        }
    }
    cout<<"Total Number of random bits generated is "<<i<<endl;
    //for (int j = i - 1; j >= 0; j--)
        //cout<<rand_bit[j];
    return rand_bit;
}

//==========THIS FUNCTION ARRANGES THE BITS INTO THE MATRIX============
int** matrixCreation(int row, int column, int *random_bits)
{
    //cout<<endl<<"Matrix Creation function"<<endl;
    int i = row * column;
    for (int j = i - 1; j >= 0; j--)
        cout<<random_bits[j];
    cout<<endl;
    //placing bits into the matrix format
    int** matrix2D = 0;
    matrix2D = new int *[row];
    int cnt = i;
    for(int r=0; r<row; r++)
    {
        matrix2D[r] = new int [column];
        for(int c=0;c<column;c++)
        {
            matrix2D[r][c] = random_bits[cnt];
            cnt--;
        }
    }
    return matrix2D;
}

//========FUNCTION THAT PERFORMS K.x MOD 2============================
int weak_prf(int** sk, int* inp, int l)
{
    int final_sum = 0;
    int out[10];
    for(int i=0;i<l;i++)
    {
        int sum = 0;
        for(int j=0;j<l;j++)
        {
            sum += ((sk[i][j])*inp[j]);
        }
        out[i] = sum%2;
    }
    cout<<endl<<"Rewriting the output as 0/1 value ";
    for(int f=0;f<l;f++)
    {
        cout<<out[f];
        final_sum += out[f];
    }
    cout<<endl<<"The final output of the weak PRF (after applying additive mod 3 is "<<(final_sum % 3)<<endl;
    return 0;
}
int main()
{
    
    cout<<"Enter a value for security parameter (lambda) "<<endl;
    cin>>lambda;
    
    cout<<"Accordingly the value of m and n are chosen; m and n being of same length as that of lambda"<<endl;
    
    //Step 1: generating a random number and convert it into binary String.
    //unsigned long rand_num = generaterandom();
    //cout<<rand_num<<endl;
    
    //Step 2: Generate a random decimal and Convert the random decimal to binary bits
    total_bits = m*n;
    int* binaryNum = randomBinary(total_bits);
    //for (int j = total_bits - 1; j >= 0; j--)
    //    cout<<binaryNum[j];
    
    //Step 3:creating a key matrix
    //cout<<"The generated matrix is"<<endl;
    int** secret_key = matrixCreation(m, n, binaryNum);
    //cout<<"Back to main function "<<endl;
    /*cout<<"The secret key matrix is generated successfully"<<endl;
    for(int x = 0; x < m ; x++)
    {
        for(int y = 0; y < n ; y++)
        {
            cout<<secret_key[x][y]<<"\t";
        }
        cout<<endl;
    }*/
    weak_prf(secret_key,input,lambda);
    return 0;
}
