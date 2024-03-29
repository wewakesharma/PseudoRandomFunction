//
//  pi.cpp
//  PRF
//
//  Created by Vivek Sharma on 1/19/20.
//  Copyright © 2020 vivek. All rights reserved.
//

#include <iostream>
#include <vector>
#include <bitset>
#include <string>
#include <chrono> 
#include <ctime> 
#include <cstdlib>//important include for the srand functions
//#include <iomanip> //This include will take care of the setprecision
using namespace std;

int size;

//string key;
//string input_val;
//vector<bitset<1>> d0, d1;

vector<bitset<1> > K;
vector<bitset<1> > X;
vector<bitset<1> > H;
vector<int> C;
vector<bitset<1> > c;
vector<int > D;

vector<bitset<1> > k1;
vector<bitset<1> > k2;
vector<bitset<1> > k3;

vector<bitset<1> > x1;
vector<bitset<1> > x2;
vector<bitset<1> > x3;

vector<bitset<1> > h1;
vector<bitset<1> > h2;
vector<bitset<1> > h3;

//Generate the value of K, randomly
void generateInput()
{
    srand(time(NULL));
    for(int i = 0; i < size; i++)
    {
        K.push_back(rand() & 1);
        X.push_back(rand() & 1);
    }
}

void setshare()
{
    //K: Randomly selects the value of k1 and k2 and perform XOR with K to set the value of k3
    srand(time(NULL));
    for(int i = 0; i < size; i++)
    {
        k1.push_back(rand() & 1);
        k2.push_back(rand() & 1);
    }
    for (int i = 0; i < size; i++)
    {
        k3.push_back(K.at(i) ^ k1.at(i) ^ k2.at(i));
    }
    
    //X: Randomly selects the value of x1 and x2 and perform XOR with X to set the value of x3
    //srand(time(NULL));
    for(int i = 0; i < size; i++)
    {
        x1.push_back(rand() & 1);
        x2.push_back(rand() & 1);
    }
    for (int i = 0; i < size; i++)
    {
        x3.push_back(X.at(i) ^ x1.at(i) ^ x2.at(i));
    }
    //C: Generating a random C belonging to {0,1,2} and then converting it into 2-bit representation.
    int a[2];
    int temp,t;
    srand(time(NULL));
    for(int i = 0; i < size; i++)
    {
        temp = rand() % 3;
        C.push_back(temp);
        t = temp % 2;
        temp = temp / 2;
        c.push_back(temp % 2);
        c.push_back(t);
    }

    //Printing the value of C and corresponding c0 and c1. c0 and c1 are 2-bit binary representation of C.
    /*cout<<"The value of c is ";
    for(int i = 0; i < size; i++)
    {
        cout<<C.at(i);
    }*/
}


    
/*Each Server locally computing the value of h i
Using the formula given in the paper. Each server Si has xj and hj i!=j
*/
void S()
{
    for(int i = 0; i < size; i++)
    {
        h1.push_back((x2.at(i) & k3.at(i)) ^ (x3.at(i) & k2.at(i)) ^ (x2.at(i) & k2.at(i)));
        h2.push_back((x1.at(i) & k3.at(i)) ^ (x3.at(i) & k1.at(i)) ^ (x3.at(i) & k3.at(i)));
        h3.push_back((x1.at(i) & k2.at(i)) ^ (x2.at(i) & k1.at(i)) ^ (x1.at(i) & k1.at(i)));
        H.push_back(h1.at(i) ^ h2.at(i) ^ h3.at(i));
    }
}

/*The subprotocol which runs exactly as shown in figure 1 in page 29 of dark crypto paper. 
Server 1 randomly selects C and Server 2 computes D. Addition modulo 3 of C and D is same as the function
*/
void pi_23()
{
    vector<bitset<1> > d0, d1;
    //vector<bitset<1>> comb;
    vector<bitset<1> > num_const ;
    int d0_int,d1_int,d_int;
    num_const.push_back(1);
    num_const.push_back(2);
    int j = 0;
    //cout<<endl<<"Value of computed d is ";
    for(int i = 0; i < size; i++)
    {
        //comb.push_back(one.at(0) ^ h1.at(i) ^ h2.at(i) ^ h3.at(i));
        d0.push_back(
                      (c.at(j) & (h1.at(i) ^ h2.at(i) ^ h3.at(i))) ^
                      (c.at(j+1) & (num_const.at(0) ^ H.at(i)))
                      );
        d1.push_back(
                     (c.at(j) ^ (num_const.at(0) ^ c.at(j+1)) &
                     (H.at(i))));
        //cout<<endl<<"d0 "<<d0.at(i)<<" d1 "<<d1.at(i);
        d0_int = (int)d0.at(i).to_ulong();
        d1_int = (int)d1.at(i).to_ulong();
        //int d1_int = (int)d1.at(i);
        d_int = d0_int * 2 + d1_int;
        //cout<<d0.at(i)<<d1.at(i);
        D.push_back(d_int);
        j = j + 2;
    }
    /*cout<<endl<<"The value of D is ";
    for(int i = 0; i < size; i++)
    {
        cout<<D.at(i);
    }
    cout<<endl;*/
}

/*The final step compares the addition modulo 3 of C and D with the secret share of input of three servers*/
void verify_pi()
{
    int h_total = 0; 
    int cd_total = 0;
    int c_int ,d_int;
    int h_int, cd_int;
    //std::vector<int> CD;
    for(int i = 0; i < size; i++)
    {
        h_int = (int)H.at(i).to_ulong();
        cd_int = C.at(i) + D.at(i);
        h_total = h_total + h_int;
        cd_total = cd_total + cd_int;
        //cout<<"h "<<(h_int % 3)<<endl;
        //cout<<"c+d "<<(cd_int % 3)<<endl;
    }
    cout<<"map(H) is "<<(h_total % 3)<<endl;    
    cout<<"map(C+D) is "<<(cd_total % 3)<<endl;
}

int main()
{
    //char choice;
    cout<<"This code will perform pi protocol among three servers"<<endl;
    cout<<"Enter the size of array"<<endl;
    cin>>size;
    //generateKey();
    //cout<<"Do you want to display the values(y/n)";
    //cin>>choice;
    chrono::time_point<std::chrono::system_clock> start, end; 
    start = chrono::system_clock::now(); 
    for(int i=0;i<10;i++){
    generateInput();
        for(int i=0;i<100;i++) {
            setshare();
            /*if(choice == 'y' || choice == 'Y')
            {
                display_values();
            }*/
            S();
            pi_23();
        }
   // verify_pi();
    //cout<<endl;
}
    end = chrono::system_clock::now();
    chrono::duration<double> elapsed_seconds = end - start; 
    time_t end_time = chrono::system_clock::to_time_t(end); 
  
    cout << "Finished at " << ctime(&end_time) 
              << "elapsed time: " << elapsed_seconds.count() << "s\n"; 
}
