//Header file weak23prf.h

#ifndef UNIT_TEST_H
#define UNIT_TEST_H

void generate_input(uint64_t*, std::mt19937, int);
void generate_rand_key(uint64_t (&key)[4][256], std::mt19937 generator);
void generate_rand_matrix(uint64_t (&randMat1)[2][256], uint64_t (&randMat2)[2][256], std::mt19937 &generator);
void compute(uint64_t (&key)[4][256], uint64_t (&input)[4], uint64_t (&z_final)[4]);
void mat_vec_mult(uint64_t (&input)[4], uint64_t (&key)[4][256], uint64_t (&out)[256]);
void phase1_test(uint64_t (&z_final)[4], uint64_t (&out)[256]);
#endif