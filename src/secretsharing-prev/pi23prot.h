//Header file pi23prot.h

//#ifndef PI23_PROT_H
//#define PI23_PROT_H

//functions for dealing with global variables
<<<<<<< HEAD
void getMaMb(uint64_t Ma[256][256], uint64_t Mb[256]);
void getMx(uint64_t mX[256]);
void sendMaMb(uint64_t Ma[256][256], uint64_t Mb[256]);
void sendMx(uint64_t Mx[256]);
void getmxbit(uint64_t mx);
void sendm0m1(uint64_t M0[256],uint64_t M1[256]);
void sendm0m1bit(uint64_t m0[256],uint64_t m1[256]);
void getrarb(uint64_t ra, uint64_t rb);
void getm0m1(uint64_t m0[256],uint64_t m1[256]);
void getm0m1bit(uint64_t m0,uint64_t m1);
=======
void multProtP2Part1(uint64_t X[4], uint64_t Rx[4]);
//void sendMx(uint64_t Mx[4]);
void multProtP1(uint64_t A[4][256], uint64_t B[4], uint64_t Ra[4][256], uint64_t Rb[4]);
//void getMx(uint64_t mX[4]);
//void sendMaMb(uint64_t Ma[4][256], uint64_t Mb[4]);

//void getMaMb(uint64_t Ma[4][256], uint64_t Mb[4]);
void multProtP2Part2(uint64_t X[4], uint64_t Z[4], uint64_t poly_eval_2PC[4]);

void poly_eval_global(uint64_t A[4][256], uint64_t X[4], uint64_t B[4], uint64_t global_res[4]);
void poly_eval_test(uint64_t poly_eval_2PC[4], uint64_t global_res[4]);
>>>>>>> cd6f466ade8684602c5c08e4f46c24663767b4c8



void PreProcGenVals(uint64_t Ra[256][256], uint64_t Rb[256], uint64_t Rx[256], uint64_t Z[256], std::mt19937 &generator);

void AXplusB_P2Part1(uint64_t X[256], uint64_t Rx[256], uint64_t Z[256]);
void AXplusB_P2Part2(uint64_t X[256], uint64_t Rx[256], uint64_t Z[256], uint64_t out[256]);
void AXplusB_P1(uint64_t A[256][256], uint64_t B[256], uint64_t Ra[256][256], uint64_t Rb[256], uint64_t out[256]);

void sc23_p1(uint64_t y1[256], uint64_t ra[256], uint64_t rb[256], uint64_t out[256], std::mt19937 &generator );
uint64_t sc23_p1Bit(uint64_t y1, uint64_t r );
void sc23_p2Part1bit(uint64_t y2);
uint64_t sc23_p2Part2bit(uint64_t y2);

void getInputVars(uint64_t A1[256],uint64_t A2[256],uint64_t X1[256][256],uint64_t X2[256][256]);

//#endif