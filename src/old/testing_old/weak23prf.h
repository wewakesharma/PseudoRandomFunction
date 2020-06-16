//Header file weak23prf.h

#ifndef UNIT_TEST_H
#define UNIT_TEST_H


//generating input
void generate_input(uint64_t*, std::mt19937, int);
void generate_rand_key(uint64_t (&key)[4][256], std::mt19937 generator);
void generate_rand_matrix(uint64_t (&randMat1)[2][256], uint64_t (&randMat2)[2][256], std::mt19937 &generator);

//phase 1 testing
void compute(uint64_t (&key)[4][256], uint64_t (&input)[4], uint64_t (&z_final)[4]);
void mat_vec_mult(uint64_t (&input)[4], uint64_t (&key)[4][256], uint64_t (&out)[256]);
void phase1_test(uint64_t (&z_final)[4], uint64_t (&out)[256]);

//phase 3-method 1 testing
void mat_assemble(uint64_t (&msbs)[2][256], uint64_t (&lsbs)[2][256], uint64_t (&z3_mat)[81][256]);
//void addMod3(uint64_t outM, uint64_t outL, uint64_t msb1, uint64_t lsb1, uint64_t msb2, uint64_t lsb2);
void multMod3(uint64_t (outM)[2], uint64_t (outL)[2], uint64_t (&msbs)[2][256], uint64_t (&lsbs)[2][256], uint64_t (in)[4], uint64_t (p3_out)[81]);
void phase3_naive(uint64_t out[256], uint64_t (&z3_mat)[81][256], uint64_t phase3_out[81]);
void phase3_test(uint64_t phase3_out[81], uint64_t p3_out[81]);

#endif