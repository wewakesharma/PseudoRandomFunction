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
#include <chrono>
#include <ctime>

//#include "mains.hpp"

#include "Toeplitz-by-x.hpp"
#include "data_receiver.h"

#define PORT 8080
char buffer[1500];
int valread;

//using namespace std;
//create a function that initializes the receiver communication: will be called just once by party 1

void init_party1_communication(int& recv_sock)
{
    int server_fd, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((recv_sock = accept(server_fd, (struct sockaddr *)&address,
                             (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
}

//change the return type based on the alternative used(alt 1: std::string and alt2:std::vector<unsigned int>
void pi_recv(int& recv_socket)//receiver uses a socket handle to receive data
{

    std::cout<<"In pi_recv function: Waiting for value to be received"<<std::endl;
    valread = read( recv_socket ,buffer, 1500);
    /*auto curr_time = std::chrono::system_clock::now();
    std::time_t recv_time = std::chrono::system_clock::to_time_t(curr_time);

    std::cout<<std::endl<<"Received value at "<<std::ctime(&recv_time)<<std::endl;*/
    printf("%s\n",buffer );

#ifdef DEBUG
    std::cout << "in pi_recv, recd_vec=" << rec_vec << std::endl;
#endif


}
