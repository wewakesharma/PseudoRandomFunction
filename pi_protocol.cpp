//
//  pi.cpp
//  PRF
//
//  Created by Vivek Sharma on 1/19/20.
//  Copyright © 2020 vivek. All rights reserved.
//

#include <iostream>
#include <vector>
#include <string>
using namespace std;

int size;

string key;
string input_val;

vector<bitset<1>> K;
vector<bitset<1>> X;

vector<bitset<1>> k1;
vector<bitset<1>> k2;
vector<bitset<1>> k3;

vector<bitset<1>> x1;
vector<bitset<1>> x2;
vector<bitset<1>> x3;

void store_value(string k, string x)
{
    for (int i = 0; i<k.size();i++)
    {
        K.push_back(k[i] - '0');
        X.push_back(x[i] - '0');
    }
}

void setshare()
{
    //K: Randomly selects the value of k1 and k2 and perform XOR with K to set the value of k3
    srand(time(NULL));
    for(int i=0; i<size; i++)
    {
        k1.push_back(rand() & 1);
        k2.push_back(rand() & 1);
    }
    for (int i=0;i<size;i++)
    {
        k3.push_back(K.at(i) ^ k1.at(i) ^ k2.at(i));
    }
    
    //X" Randomly selects the value of x1 and x2 and perform XOR with X to set the value of x3
    srand(time(NULL));
    for(int i=0; i<size; i++)
    {
        x1.push_back(rand() & 1);
        x2.push_back(rand() & 1);
    }
    for (int i=0;i<size;i++)
    {
        x3.push_back(X.at(i) ^ x1.at(i) ^ x2.at(i));
    }
}

void display_values()
{
    //Displaying the values of k1,k2, and k3
    cout<<"The value of k1 is ";
    for (int i=0; i<k1.size();i++)
    {
        cout<<k1.at(i);
    }
    cout<<endl<<"The value of k2 is ";
    for (int i=0; i<k2.size();i++)
    {
        cout<<k2.at(i);
    }
    cout<<endl<<"The value of k3 is ";
    
    for (int i=0; i<k3.size();i++)
    {
        cout<<k3.at(i);
    }
    cout<<endl;
    //Displaying the value of x1,x2, and x3
    cout<<"The value of x1 is ";
    for (int i=0; i<x1.size();i++)
    {
        cout<<x1.at(i);
    }
    cout<<endl<<"The value of x2 is ";
    for (int i=0; i<x2.size();i++)
    {
        cout<<x2.at(i);
    }
    cout<<endl<<"The value of x3 is ";
    for (int i=0; i<x3.size();i++)
    {
        cout<<x3.at(i);
    }
    cout<<endl;
}
    

int pi_protocol()
{
    return 0;
}
int pi_two_three_protocol()
{
    return 0;
}
int main()
{
    cout<<"This code will perform pi protocol among three servers"<<endl;
    cout<<"Enter the size of array"<<endl;
    cin>>size;//keep it three at this time
    cout<<"Enter the key value k of length "<<size<<endl;
    cin>>key;
    cout<<"Enter the input value x of length "<<size<<endl;
    cin>>input_val;
    store_value(key,input_val);
    setshare();
    display_values();
}