//Header file weak23prf.h

#ifndef UNIT_TEST_H
#define UNIT_TEST_H

const int wLen=64;

//generating input
void generate_rand_input(uint64_t input[4], std::mt19937 &generator);
void generate_rand_key(uint64_t key[4][256], std::mt19937 &generator);
void generate_rand_Z3_matrix_81x256(uint64_t randMat1[2][256], uint64_t randMat2[2][256], uint64_t randMatZ3[81][256], std::mt19937 &generator);
void generate_test_Z3_matrix_81x256(uint64_t randMat1[2][256], uint64_t randMat2[2][256], uint64_t randMatZ3[81][256], std::mt19937 &generator);

//phase 1 testing
void wordPackedVecMatMult(uint64_t key[4][256], uint64_t input[4], uint64_t z_final[4]);
void unpackOutput(uint64_t output[4], char p2output[256]);

//phase 3-method 1 testing
void multMod3_Z3Mat_Z2Vec(uint64_t outM[2], uint64_t outL[2], uint64_t msbs[2][256], uint64_t lsbs[2][256], uint64_t in[4]);
void multMod3_Z3Mat_Z3Vec(uint64_t msbs[2][256], uint64_t lsbs[2][256], uint64_t inm[4], uint64_t inl[4], uint64_t outM[2], uint64_t outL[2]);
void multMod3_Z3Mat_Z3Vec2(uint64_t msbs[2][256], uint64_t lsbs[2][256], uint64_t inm[4], uint64_t inl[4], uint64_t outM[2], uint64_t outL[2]);

void addMod3(uint64_t& outM, uint64_t& outL, uint64_t msb1, uint64_t lsb1, uint64_t msb2, uint64_t lsb2);
void subMod3(uint64_t& outM, uint64_t& outL, uint64_t msb1, uint64_t lsb1, uint64_t msb2, uint64_t lsb2);

//PRF alternate method testing
void InnerProdMul(uint64_t outVec[84], uint64_t randMatZ3[128][256], uint64_t in[4]);
void InnerProdMul2(uint64_t outVec[84], uint64_t randMatZ3[128][256], uint64_t in[4]);

void addMod3vec4(uint64_t msb1[4], uint64_t lsb1[4], uint64_t msb2[4], uint64_t lsb2[4], uint64_t outM[4], uint64_t outL[4]);
void addMod3vec2(uint64_t msb1[2], uint64_t lsb1[2], uint64_t msb2[2], uint64_t lsb2[2], uint64_t outM[2], uint64_t outL[2]);



#endif