//
// Created by Vivek Sharma on 7/13/20.
//

#ifndef SECRETSHARING_PI_UNIT_TEST_H
#define SECRETSHARING_PI_UNIT_TEST_H

void poly_eval_global(uint64_t A[4][256], uint64_t X[4], uint64_t B[4], uint64_t global_res[4]);
void poly_eval_test(uint64_t out[4], uint64_t global_res[4]);

void generate_bits_4(uint64_t x_bits[4], std::mt19937 &generator);
void generate_msb_lsb_Z3(uint64_t z3_value[4], uint64_t msb[4], uint64_t lsb[4], std::mt19937 &generator);
#endif //SECRETSHARING_PI_UNIT_TEST_H
