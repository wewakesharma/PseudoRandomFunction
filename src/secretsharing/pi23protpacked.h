//
// Created by Vivek Sharma on 7/6/20.
//

#ifndef PI23PROTPACKED_H
#define PI23PROTPACKED_H

void multProtP1(uint64_t A[256][256], uint64_t B[256], uint64_t Ra[256][256], uint64_t Rb[256], uint64_t out[2]);
void multProtP2Part1(uint64_t X[256], uint64_t Rx[256], uint64_t Z[256], uint64_t out[256]);
void multProtP2Part2(uint64_t X[256], uint64_t Rx[256], uint64_t Z[256], uint64_t out[256]);
//void generate_randkey_pi_protocol(uint64_t key[256][256], std::mt19937 &generator);
//void generate_two_word_randinput(uint64_t input[256], std::mt19937 &generator);

#endif //PI23PROTPACKED_H
