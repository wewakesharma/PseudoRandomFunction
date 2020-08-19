//
// Created by Vivek Sharma on 8/6/20.
//

#ifndef INC_2PC_DATA_SENDER_H
#define INC_2PC_DATA_SENDER_H

void init_send_connection(int& port);
void pi_snd(std::vector<unsigned int>& snd_vec);
void close_client_connection(int& clientSd);

#endif //INC_2PC_DATA_SENDER_H
