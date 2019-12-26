//
//  main.cpp
//  PRF
//
//  Created by Vivek Sharma on 12/25/19.
//  Copyright © 2019 vivek. All rights reserved.
//

/*#include <iostream>

using namespace std;
int main(int argc, const char * argv[]) {
    // insert code here...
    cout << "Hello, World!\n";
    return 0;
}
*/

//Mod-2/Mod-3 implementation of Pseudorandom Function
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include<typeinfo>
//#include<stdlib>
using namespace std;

long generaterandom()//It will generate random numbers
{
    srand(time(NULL));
    int rand_num = rand();
    cout<<"Original Random Number is "<<rand_num<<endl;
    rand_num = rand_num;
    return rand_num;
}


void matrixCreation(unsigned long num,int row, int col)//This code is taken from geeksforgeeks
{
    int secret_key[row][col];
    cout<<"The value of random number is "<<num<<endl;
    int binaryNum[32];
    int i = 0;
    while (num > 0)
    {
        binaryNum[i] = num % 2;
        num = num / 2;
        i++;
    }
    for (int j = i - 1; j >= 0; j--)
        cout<<binaryNum[j];
    cout<<endl<<"Number of rows for the matrix is "<<row<<endl;
    cout<<"Number of columns for the matrix is "<<col<<endl;
    cout<<"Total Number of elements in matrix must be "<<(row*col)<<endl;
    cout<<"Total Number of elements we have generated is "<<i<<endl;
    if(i<(row*col))
    {
        cout<<"Sorry, the bits fell short to fill up your matrix, try increasing the size in generation"<<endl;
    }
    else//create the secret key matrix
    {
        int cnt = i-1;
        for (int r = 0; r<row; r++)
        {
            for(int c = 0; c<col; c++)
            {
                secret_key[r][c] =binaryNum[cnt];
                cnt--;
                //if (i<0)
                    //break;
            }
        }
    }
    //printing the secret key
    cout<<"The secret key matrix A is "<<endl;
    for(int r = 0;r<row;r++)
    {
        for(int c= 0;c<col;c++)
        {
            cout<<secret_key[r][c]<<"\t";
        }
        cout<<endl;
    }
    int x[4] = {0,1,1,0};
    int y_bar[row];
    //int sum = 0;
    for(int i=0;i<row;i++)
    {
        int sum = 0;
        for(int j=0;j<col;j++)
        {
            sum += ((secret_key[i][j])*x[j]);
        }
        y_bar[i] = sum;
    }
    for(int c=0;c<row;c++)
    {
        cout<<y_bar[c];
    }
    cout<<endl;
    int y_bar_sum = 0;
    for(int c=0;c<row;c++)
    {
        y_bar_sum += y_bar[c];
    }
    cout<<"The output of this weak PRF is "<<(y_bar_sum % 3)<<endl;
}

int truncate(int key, int len)//This function will truncate the length of any entity to a defined
{
    return 0;
}

int main()
{
    int lambda;
    
    cout<<"Enter a value for security parameter (lambda) "<<endl;
    cin>>lambda;
    int m = lambda;
    int n = lambda;
    //int key[m][n];
    cout<<"Accordingly the value of m and n are chosen; m and n being of same length as that of lambda"<<endl;
    //creating a key
    //int key_matrix[m][n];
    unsigned long rand_num = generaterandom();
    cout<<rand_num<<endl;
    matrixCreation(rand_num,m,n);
    //cout<<"The binary of number "<<nm<<" is "<<decToBinary(nm)<<endl;
    return 0;
}