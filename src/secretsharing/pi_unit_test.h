//
// Created by Vivek Sharma on 7/13/20.
//

#ifndef SECRETSHARING_PI_UNIT_TEST_H
#define SECRETSHARING_PI_UNIT_TEST_H

void poly_eval_global(uint64_t A[4][256], uint64_t X[4], uint64_t B[4], uint64_t global_res[4]);
void poly_eval_test(uint64_t out[4], uint64_t global_res[4]);

void generate_bits_4(uint64_t x_bits[4], std::mt19937 &generator);
void generate_msb_lsb_Z3(uint64_t z3_value[4], uint64_t msb[4], uint64_t lsb[4], std::mt19937 &generator);
void OT_test(uint64_t wm[4],uint64_t wl[4],uint64_t X[4],uint64_t r0m[4],uint64_t r0l[4],uint64_t r1m[4],uint64_t r1l[4]);
void OTPreproc_debug(uint64_t ram[4],uint64_t ral[4],uint64_t rbm[4],uint64_t rbl[4],uint64_t rx[4],uint64_t zm[4],uint64_t zl[4],std::mt19937 &generator);
#endif //SECRETSHARING_PI_UNIT_TEST_H
