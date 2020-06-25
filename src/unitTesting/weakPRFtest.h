//
// Created by Vivek Sharma on 6/24/20.
//

#ifndef SRC_WEAKPRFTEST_H
#define SRC_WEAKPRFTEST_H

const int wLen = 64;
void mat_vec_mult(uint64_t (&input)[4], uint64_t (&key)[4][256], uint64_t (&naive_out_p1)[256]);
void phase1_test(uint64_t (&z_final)[4], uint64_t (&naive_out_p1)[256]);

void merge_out_LM(uint64_t (&outM)[2], uint64_t (&outL)[2], uint64_t (&p3_out)[81]);
void phase3_naive(uint64_t (&naive_out_p1)[256], uint64_t (&randMatZ3)[81][256], uint64_t (&naive_out_p3)[81]);
void phase3_test(uint64_t (&naive_out_p3)[81], uint64_t (&p3_out)[81]);

void InnerProdMul_test(uint64_t (&outVec)[84], uint64_t (&naive_out_p3)[81]);
void InnerProdMul2_test(uint64_t (&outVec)[84], uint64_t (&naive_out_p3)[81]);

#endif //SRC_WEAKPRFTEST_H
