//Header file weak23prf.h

//#ifndef UNIT_TEST_H
//#define UNIT_TEST_H

//functions for dealing with global variables

void PreProcPackedGenVals(uint64_t Ra[4][256], uint64_t Rb[4], uint64_t Rx[4], uint64_t Z[4], std::mt19937 &generator);

void AXplusB_P2PackedPart1(uint64_t X[4], uint64_t Rx[4], uint64_t Z[4]);
void AXplusB_P1Packed(uint64_t A[4][256], uint64_t B[4], uint64_t Ra[4][256], uint64_t Rb[4]);
void AXplusB_P2PackedPart2(uint64_t X[4], uint64_t Rx[4], uint64_t Z[4], uint64_t out[4]);

void getInputPackedVars(uint64_t A1[4][256],uint64_t A2[4][256],uint64_t X1[4],
                        uint64_t X2[4], std::mt19937 &generator);
//void getInputPackedVars(uint64_t A1[4][256],uint64_t A2[4][256],uint64_t X1[4],uint64_t X2[4]);

//testing functions
void poly_eval_global(uint64_t A[4][256], uint64_t X[4], uint64_t B[4], uint64_t global_res[4]);
void poly_eval_test(uint64_t poly_eval_2PC[4], uint64_t global_res[4]);

void sc23_p2Part1Packed(uint64_t Y2[4], uint64_t Rx[4] );
void sc23_p1Packed(uint64_t y1[4], uint64_t vm[4],  uint64_t vl[4], std::mt19937 &generator);
void sc23_p2Part2Packed(uint64_t Rx[4], uint64_t Zm[4], uint64_t Zl[4], uint64_t Wm[4], uint64_t Wl[4] );


void getrarbPacked(uint64_t ra[4], uint64_t rb[4]);
void getMxMulPacked(uint64_t mX[4]);
void getMxOTPacked(uint64_t mX[4]);
void sendMxMulPacked(uint64_t MxPacked[4]);
void sendMxOTPacked(uint64_t MxPacked[4]);;
void getm0m1Packed(uint64_t m0m[4],uint64_t m0l[4], uint64_t m1m[4],uint64_t m1l[4]);



void PreProcPackedGenVals(uint64_t Ra[4][256], uint64_t Rb[4], uint64_t Rx[4], uint64_t Z[4], std::mt19937 &generator);

void OTZ3_R_Part1Packed(uint64_t x[4], uint64_t rx[4]);
void OTZ3_R_Part2Packed(uint64_t Rx[4], uint64_t Zm[4], uint64_t Zl[4], uint64_t Wm[4], uint64_t Wl[4]);
void OTZ3_S_Packed(uint64_t r0m[4], uint64_t r1m[4], uint64_t ram[4], uint64_t rbm[4],uint64_t r0l[4], uint64_t r1l[4], uint64_t ral[4], uint64_t rbl[4]);void sendm0m1Z3Packed(uint64_t m0[4],uint64_t m1[4]);

void addMod3(uint64_t& outM, uint64_t& outL, uint64_t msb1, uint64_t lsb1, uint64_t msb2, uint64_t lsb2);
void getInputPackedVars(uint64_t A1[4][256],uint64_t A2[4][256],uint64_t X1[4],uint64_t X2[4], std::mt19937 &generator);

//#endif