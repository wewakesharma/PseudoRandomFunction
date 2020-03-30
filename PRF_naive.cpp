#include <iostream>
#include <chrono> 
#include <ctime> 
#include <cstdlib>

using namespace std;

int input[256];
int** secret_key;						//A pointer to pointers to an int.
int rows,columns,lambda;

void generateInput()
{
	srand(time(NULL));
    for(int i = 0; i < lambda; i++)
    {
        input[i] = rand() & 1;
    }
}

void matrixCreation()
{
	secret_key=new int*[rows];				//Matrix is now a pointer to an array of 'rows' pointers.
	
	//define the matrix
	srand(time(NULL));
	for(int i=0;i<rows;i++)
	{
		secret_key[i]=new int[columns];		//the ith array is initialized
		for(int j=0;j<columns;j++)		//the i,jth element is defined
		{
			secret_key[i][j] = rand() & 1;
		}
	}
}

int weak_prf()
{
    int final_sum = 0;
    int out[256];
    for(int i=0;i<lambda;i++)
    {
        int sum = 0;
        for(int j=0;j<lambda;j++)
        {
            sum += ((secret_key[i][j])*input[j]);
        }
        out[i] = sum%2;
    }
    cout<<endl<<"Rewriting the output as 0/1 value ";
    for(int f=0;f<lambda;f++)
    {
        //cout<<out[f];
        final_sum += out[f];
    }
    cout<<endl<<"The final output of the weak PRF (after applying additive mod 3 is "<<(final_sum % 3)<<endl;
    return 0;
}

int main()
{
	cout<<"Enter the security parameter  ";
	cin>>lambda;
	rows = lambda;
	columns = lambda;
	chrono::time_point<std::chrono::system_clock> start, end; 
    start = chrono::system_clock::now(); 
    for(int i=0;i<10;i++)
    {
		generateInput();
		for(int j=0;j<100;j++) 
		{
			matrixCreation();
			weak_prf();
		}	
	}
	end = chrono::system_clock::now();
    chrono::duration<double> elapsed_seconds = end - start; 
    time_t end_time = chrono::system_clock::to_time_t(end); 
  
    cout << "Finished at " << ctime(&end_time) 
              << "elapsed time: " << elapsed_seconds.count() << "s\n";
}