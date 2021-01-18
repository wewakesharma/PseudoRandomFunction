//
// Created by Vivek Sharma on 1/18/21.
//

#ifndef NEWPROTOCOL_INTEGER_PACK_H
#define NEWPROTOCOL_INTEGER_PACK_H

void pack_matrix(vector<vector<uint64_t> >& Rmat14, std::vector<PackedZ3<81> >& Rmat);
void unpack_output(std::vector<uint64_t>& out_Z3, std::vector<uint64_t>& out_pack);
void matByVec_int_pack(std::vector<uint64_t>& out_Z3, PackedZ2<N_COLS>& outKX, std::vector<vector<uint64_t> >& Rmat14);

#endif //NEWPROTOCOL_INTEGER_PACK_H
