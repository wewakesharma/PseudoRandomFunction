//
// Created by Vivek Sharma on 8/6/20.
//

#ifndef INC_2PC_DATA_RECEIVER_H
#define INC_2PC_DATA_RECEIVER_H

//change the return type based on the alternative used(alt 1: std::string and alt2:std::vector<unsigned int>
void init_party1_communication(int& recv_sock);
void pi_recv(int& recv_socket);

#endif //INC_2PC_DATA_RECEIVER_H
