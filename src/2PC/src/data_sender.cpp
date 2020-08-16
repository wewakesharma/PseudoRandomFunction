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
#include "mains.hpp"

#include "Toeplitz-by-x.hpp"
#include "data_sender.h"

using namespace std;

void pi_snd(std::vector<unsigned int>& snd_vec)
{
    char const *serverIp = "127.0.0.1";
    int port = 12345;
    //create a message buffer
    //std::string msg;
    char msg[1500];
    //setup a socket and connection tools
    struct hostent* host = gethostbyname(serverIp);
    sockaddr_in sendSockAddr;
    bzero((char*)&sendSockAddr, sizeof(sendSockAddr));
    sendSockAddr.sin_family = AF_INET;
    sendSockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    sendSockAddr.sin_port = htons(port);
    int clientSd = socket(AF_INET, SOCK_STREAM, 0);
    //try to connect...
    int status = connect(clientSd,(sockaddr*) &sendSockAddr, sizeof(sendSockAddr));
    if(status < 0)
    {
        std::cout<<"Error connecting to socket!"<<std::endl;
        //break;
    }
    std::cout << "Connected to the server!" << std::endl;
    std::cout << ">";

    //Enable only one of the following alternates to send the message

    //ALTERNATE 1=========Working Code: Uint vector -> ostringstream -> string=======
   // memset(&msg, 0, sizeof(msg));//clear the buffer

   /*
    std::ostringstream oss;

    if (!snd_vec.empty())
    {
        // Convert all but the last element to avoid a trailing ","
        std::copy(snd_vec.begin(), snd_vec.end()-1,
                  std::ostream_iterator<int>(oss, ""));

        // Now add the last element with no delimiter
        oss << snd_vec.back();
    }

    //strcpy(msg, oss.str().c_str());
   // send(clientSd, (char*)&msg, strlen(msg), 0);
*/

    //TODO: send the vector information directly instead of the text message

    send(clientSd, (char*)&(snd_vec[0]), (sizeof(unsigned int) * snd_vec.size()), 0);


#ifdef DEBUG
    cout << "in pi_snd, snd_vec= " << snd_vec << endl;
#endif

    /*ALTERNATE 2========SEnd uint vector directly

    //memset(&msg, 0, sizeof(msg));//clear the buffer
    const unsigned int vec_size = (unsigned int)snd_vec.size();
    send(clientSd, (const void *)(vec_size), sizeof(unsigned int), 0);//why to send the size of structure, when it is fixed
    for(unsigned int i = 0; i < snd_vec.size(); i++)
    {
        send(clientSd,&(snd_vec[i]),sizeof(unsigned int),0);
    }*/


    close(clientSd);
}

