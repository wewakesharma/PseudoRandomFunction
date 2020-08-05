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
#include <fcntl.h>
#include <fstream>
#include <sstream>//convert string to integer
using namespace std;
//Server side
int main(int argc, char *argv[])
{
    //for the server, we only need to specify a port number
    /*if(argc != 2)
    {
        cerr << "Usage: port" << endl;
        exit(0);
    }*/
    //grab the port number
    //int port = atoi(argv[1]);
    int port = 12345;
    //buffer to send and receive messages with
    char msg[1500];
     
    //setup a socket and connection tools
    sockaddr_in servAddr;
    bzero((char*)&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);
 
    //open stream oriented socket with internet address
    //also keep track of the socket descriptor
    int serverSd = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSd < 0)
    {
        cerr << "Error establishing the server socket" << endl;
        exit(0);
    }
    //bind the socket to its local address
    int bindStatus = bind(serverSd, (struct sockaddr*) &servAddr, 
        sizeof(servAddr));
    if(bindStatus < 0)
    {
        cerr << "Error binding socket to local address" << endl;
        exit(0);
    }
    cout << "Waiting for a client to connect..." << endl;
    //listen for up to 5 requests at a time
    listen(serverSd, 5);
    //receive a request from client using accept
    //we need a new address to connect with the client
    sockaddr_in newSockAddr;
    socklen_t newSockAddrSize = sizeof(newSockAddr);
    //accept, create a new socket descriptor to 
    //handle the new connection with client
    int newSd = accept(serverSd, (sockaddr *)&newSockAddr, &newSockAddrSize);
    if(newSd < 0)
    {
        cerr << "Error accepting request from client!" << endl;
        exit(1);
    }
    cout << "Connected with client!" << endl;
    //lets keep track of the session time
    struct timeval start1, end1;
    gettimeofday(&start1, NULL);
    //also keep track of the amount of data sent as well
    int bytesRead, bytesWritten = 0;
    cout << "Waiting for input" << endl;
    memset(&msg, 0, sizeof(msg));//clear the buffer
    bytesRead += recv(newSd, (char*)&msg, sizeof(msg), 0);
    cout << "Number received is : " << msg << endl;
    cout << ">";
    stringstream num(msg);
    int x=0;
    num>>x;
    int square_num = 0;
    square_num = x * x;
    cout<<"Square computed by server"<<square_num;
    std::string data = std::to_string(square_num);
        //string data = (string)square_num;
    //getline(cin, data);
    memset(&msg, 0, sizeof(msg)); //clear the buffer
    strcpy(msg, data.c_str());
        //send the message to client
    bytesWritten += send(newSd, (char*)&msg, strlen(msg), 0);
    //we need to close the socket descriptors after we're all done
    gettimeofday(&end1, NULL);
    close(newSd);
    close(serverSd);
    cout << "Bytes written: " << bytesWritten << " Bytes read: " << bytesRead << endl;
    //cout << "Elapsed time: " << (end1.tv_sec - start1.tv_sec) 
        << " secs" << endl;
    return 0;   
}