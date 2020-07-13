//Description

It contains the files and functions in each file along with their description, input and output

File name: dmweakPRF.cpp
	Function name: generate_rand_input
		Input:uint64_t input[4], std::mt19937 &generator
		Output:void
	Function name: generate_rand_key
		Input:uint64_t key[4][256], std::mt19937 &generator
		Output:void
	Function name: generate_rand_matrix
		Input:uint64_t randMat1[2][256], uint64_t randMat2[2][256], uint64_t randMatZ3[81][256], std::mt19937 &generator
		Output:void
	Function name: wordPackedVecMatMult
		Input:uint64_t key[4][256], uint64_t input[4], uint64_t z_final[4]
		Output:void
	Function name: unpackOutput
		Input:uint64_t output[4], char p2output[256]
		Output:void
	Function name: multMod3
		Input:uint64_t outM[2], uint64_t outL[2], uint64_t msbs[2][256], uint64_t lsbs[2][256], uint64_t in[4]
		Output:void
	Function name: InnerProdMul
		Input:uint64_t outVec[84], uint64_t randMatZ3[128][256], uint64_t in[4]
		Output:void
	Function name: InnerProdMul2
		Input:uint64_t outVec[84], uint64_t randMatZ3[128][256], uint64_t in[4]
		Output:void
	

