//
// Created by Vivek Sharma on 8/6/20.
//

#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <typeinfo>
//#include "mains.hpp"

#include "Toeplitz-by-x.hpp"
#include "data_receiver.h"
#include "data_sender.h"
//using namespace std;

//Global Variables
int serverSd;
int newSd;

int openSocket(int& bindStatus, int& port) {

    //int port = 12345;
    //setup a socket and connection tools
    sockaddr_in servAddr;
    bzero((char *) &servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);

    //open stream oriented socket with internet address
    //also keep track of the socket descriptor
    int serverSd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSd < 0) {
        std::cerr << "Error establishing the server socket" << std::endl;
        exit(0);
    }

    std::cerr << "in data receiver OpenSocket, success opening socket" << std::endl;

    //bind the socket to its local address
    bindStatus = bind(serverSd, (struct sockaddr *) &servAddr, sizeof(servAddr));
    if (bindStatus < 0) {
        std::cerr << "in data_reeiver, opensocket, Error binding socket to local address" << std::endl;
        exit(0);
    }
    std::cerr << "in data receiver OpenSocket , success binding socket to local address" << std::endl;
    return serverSd;
}

//INITIALIZE CONNECTION - Open Socket and returns the newSd
void init_recv_connection(int& port){

    std::vector<unsigned int> rec_vec;

    int bindStatus;

    serverSd = openSocket(bindStatus,port);
    assert(bindStatus>=0);

    std::cout << "Waiting for a client to connect..." << std::endl;
    //listen for up to 5 requests at a time
    listen(serverSd, 5);
    //receive a request from client using accept
    //we need a new address to connect with the client
    sockaddr_in newSockAddr;
    socklen_t newSockAddrSize = sizeof(newSockAddr);
    //accept, create a new socket descriptor to
    //handle the new connection with client
    newSd = accept(serverSd, (sockaddr *)&newSockAddr, &newSockAddrSize);
    if(newSd < 0)
    {
        std::cerr << "Error accepting request from client!" << std::endl;
        exit(1);
    }
    std::cout << "in pi_recv, Connected with Party 2!" << std::endl;
}



//change the return type based on the alternative used(alt 1: std::string and alt2:std::vector<unsigned int>
void pi_recv()//p1 acts as a server
{
    char msg[1500];
    //buffer to send and receive messages with
    //Alternate 1 - Working code: the value are received in string
    memset(&msg, 0, sizeof(msg));//clear the buffer
    std::cout<<std::endl<<"Memory cleared! Waiting for data"<<std::endl;
    recv(newSd, (char*)&msg, sizeof(msg), 0);

#ifdef DEBUG
    std::cout << "in pi_recv, recd_vec=" << rec_vec << std::endl;
#endif

    std::cout<<msg;
    std::string msg_str(msg);
    std::cout<<msg_str<<std::endl;

}

void close_receiver_connection()
{
    close(newSd);
    close(serverSd);
    std::cout<<std::endl<<"All connection closed"<<std::endl;
}


//ARCHIVE CODE
//FOLLOWING CODE CONTAINS CONVERSION FROM STRING TO UNSIGNED INT
/*
 * convert msg from char* to string
std::string msg_str(msg);
std::vector<unsigned int> recd_mx_vec;
#ifdef DEBUG
std::cout<<msg_str<<std::endl;

std::cout << "in pi_recv, msg_str bits =" << std::endl;
for(std::string::size_type i = 0; i < msg_str.size(); ++i) {
    std:: cout << (msg_str[i]);
    recd_mx_vec.push_back((msg_str[i]) - '0');
}

std::cout<<msg_str<<std::endl;

std::cout << "in pi_recv, recd_mx_vec =" << recd_mx_vec << std::endl;


#endif

 */

//converting string into unsigned int vector

//Method 1:
/*std::istringstream iss(msg_str);
recd_mx_vec.assign( std::istream_iterator<unsigned int>( iss ), std::istream_iterator<unsigned int>() );

for(int i = 0; i < recd_mx_vec.size(); i++)
{
    std::cout<<recd_mx_vec.at(i);
}*/

//Method 2:

/*std::vector<unsigned int> recd_mx_vec;
int len = 0;
unsigned int value;
while(len < 256)
{
    std::stringstream ss(msg_str.at(len));
    value = 0;
    ss >> value;
    //std::cout<<value;
    recd_mx_vec.push_back(value);
    len++;
}
for (int j = 0; j < recd_mx_vec.size(); j++)  //Fix variables
{
    std::cout << recd_mx_vec[j] << " " ; // Can use simply v[j]
}

return recd_mx_vec;*/

//Method 3:
/* Alternate 2 -  The size of vector is received before receiving each element of vector
int size = 0;

recv(newSd, &size, sizeof(unsigned int), 0);
for(int i = 0; i < size; i++)
{
    unsigned int t; // In case it's a Float vector, change Type to float
    // In case it's a Int vector, change Type to int
    // And so on...
    recv(newSd,&t,sizeof(unsigned int),0);
    recd_mx.push_back(t);
}
return recd_mx;*/

//Method 4: Directly saving string stream to values==Doesn't work
/*
int len = 0;
std::vector<unsigned int> recd_mx_vec;
//unsigned int value;
while(len < 256) {
    std::stringstream ss(msg_str.at(len));
    int value;
    while (ss >> value) {
        recd_mx_vec.push_back(value);
    }
    len++;
return recd_mx_vec;

//Method 5: Naive method

std::vector<unsigned int> recd_mx_vec;
int len = 0;
unsigned int value;
while(len < 256)
{
    char bit = msg_str.at(len);
    if(bit == 1)
        recd_mx_vec.push_back(0x00000001);
    else
        recd_mx_vec.push_back(0);
    len++;
}


#ifdef DEBUG
for (int j = 0; j < 256; j++)  //Fix variables
{
    std::cout << recd_mx_vec[j] << " " ; // Can use simply v[j]
}

std::cout << std::endl;
#endif
*/
//    return recd_mx_vec;