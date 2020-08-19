//
// Created by Vivek Sharma on 8/6/20.
//

#ifndef INC_2PC_DATA_RECEIVER_H
#define INC_2PC_DATA_RECEIVER_H

//change the return type based on the alternative used(alt 1: std::string and alt2:std::vector<unsigned int>
void init_recv_connection(int& port);
void pi_recv();
void close_receiver_connection();

#endif //INC_2PC_DATA_RECEIVER_H
