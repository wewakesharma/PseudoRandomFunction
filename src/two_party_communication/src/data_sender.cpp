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

//#include "mains.hpp"

#include "Toeplitz-by-x.hpp"
#include "data_sender.h"

#define PORT 8080

using namespace std;

void init_party2_communication(int& send_sock)
{
    send_sock = 0;
    int valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    if ((send_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        //return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        //return -1;
    }

    if (connect(send_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        //return -1;
    }
}

void pi_snd(std::vector<unsigned int>& snd_vec, int& send_sock)
{
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    //char *hello = "Hello from client";
    char msg[1500];

    //conversion from unsigned int to char*
    std::ostringstream oss;
    if (!snd_vec.empty())
    {
        // Convert all but the last element to avoid a trailing ","
        std::copy(snd_vec.begin(), snd_vec.end()-1,
                  std::ostream_iterator<int>(oss, ""));

        // Now add the last element with no delimiter
        oss << snd_vec.back();
    }

    //convert string to char
    strcpy(msg, oss.str().c_str());

    send(send_sock , msg , strlen(msg) , 0 );
    std::cout<<std::endl<<"value to be sent is "<<msg<<std::endl;
    auto curr_time = std::chrono::system_clock::now();
    std::time_t send_time = std::chrono::system_clock::to_time_t(curr_time);
    std::cout<<std::endl<<"Value sent at "<<std::ctime(&send_time)<<std::endl;

#ifdef DEBUG
    cout << "in pi_snd, snd_vec= " << snd_vec << endl;
#endif


}

