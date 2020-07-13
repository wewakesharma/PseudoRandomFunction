//Header file dmweakPRFnopack.h

//#ifndef DM_WEAK_NOPACK_H
//#define DM_WEAK_NOPACK_H

//void VecMatMultnotPack2(uint64_t key[128][256], uint64_t input[256], uint64_t z_final[128]);
//void VecMatMultnotPack(uint64_t key[256][256], uint64_t input[256], uint64_t z_final[256]);
void mat_vec_mult(uint64_t (&input)[4], uint64_t (&key)[4][256], uint64_t (&naive_out_p1)[256]);
void unpack(uint64_t (&packword)[4], uint64_t (&unpck_bits)[256]);
void unpacked_mat_vec_mult(uint64_t unpck_mat[256][256], uint64_t unpck_vec[256]);
void unpacked_mat_vec_mult(uint64_t unpck_mat[256][256], uint64_t unpck_vec[256], uint64_t unpck_mult_result [256]);

//#endif