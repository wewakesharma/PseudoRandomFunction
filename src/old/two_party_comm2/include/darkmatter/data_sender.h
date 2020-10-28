//
// Created by Vivek Sharma on 8/6/20.
//

#ifndef INC_2PC_DATA_SENDER_H
#define INC_2PC_DATA_SENDER_H

void init_party2_communication(int& send_sock);
void pi_snd(std::vector<unsigned int>& snd_vec, int& send_sock);

#endif //INC_2PC_DATA_SENDER_H
