//Header file weak23prf.h

//#ifndef UTIlS_H
//#define UTILS_H

//functions for dealing with global variables

void generate_rand_vector_256(uint64_t vec[256], std::mt19937 &generator);
void generate_rand_sqMat_256(uint64_t mat[256][256], std::mt19937 &generator);

void generate_rand_packed_vector_4(uint64_t vec[4], std::mt19937 &generator);
void generate_rand_packed_sqMat_4(uint64_t mat[4][256], std::mt19937 &generator);

void generate_rand_Z3_packed_Vec_4(uint64_t PackedVecm[4], uint64_t PackedVecl[4], uint64_t unpackedVec[256], std::mt19937 &generator);


//#endif