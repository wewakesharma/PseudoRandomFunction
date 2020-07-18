//
// Created by Vivek Sharma on 7/13/20.
//

#ifndef SECRETSHARING_PI_UNIT_TEST_H
#define SECRETSHARING_PI_UNIT_TEST_H

void poly_eval_global(uint64_t A[4][256], uint64_t X[4], uint64_t B[4], uint64_t global_res[4]);
void poly_eval_test(uint64_t out[4], uint64_t global_res[4]);

#endif //SECRETSHARING_PI_UNIT_TEST_H
