//Header file pi23prot.h

//#ifndef PI23_PROT_H
//#define PI23_PROT_H

//functions for dealing with global variables
void multProtP2Part1(uint64_t X[4], uint64_t Rx[4]);
//void sendMx(uint64_t Mx[4]);
void multProtP1(uint64_t A[4][256], uint64_t B[4], uint64_t Ra[4][256], uint64_t Rb[4]);
//void getMx(uint64_t mX[4]);
//void sendMaMb(uint64_t Ma[4][256], uint64_t Mb[4]);

//void getMaMb(uint64_t Ma[4][256], uint64_t Mb[4]);
void multProtP2Part2(uint64_t X[4], uint64_t Z[4], uint64_t poly_eval_2PC[4]);

void poly_eval_global(uint64_t A[4][256], uint64_t X[4], uint64_t B[4], uint64_t global_res[4]);
void poly_eval_test(uint64_t poly_eval_2PC[4], uint64_t global_res[4]);


//#endif