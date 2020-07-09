//Header file dmweakPRFnopack.h

//#ifndef DM_WEAK_NOPACK_H
//#define DM_WEAK_NOPACK_H

void VecMatMultnotPack2(uint64_t key[128][256], uint64_t input[256], uint64_t z_final[128]);
void VecMatMultnotPack(uint64_t key[256][256], uint64_t input[256], uint64_t z_final[256]);

//#endif