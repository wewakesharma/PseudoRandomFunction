//
// Created by Vivek Sharma on 10/21/20.
//

#ifndef NEWPROTOCOL_LOOKUP_FUNCTIONS_H
#define NEWPROTOCOL_LOOKUP_FUNCTIONS_H

void reformat_input(std::vector<uint64_t>& outKX_input, PackedZ2<256>& outKX);//converts 256 bit input(PackedZ2) to 16 vector of 16 bits each

void reformat_Rmat(std::vector<std::vector<PackedZ3<81> > >& Rmat16, std::vector<PackedZ3<81> >& Rmat);

void create_lookup_table(std::vector<std::vector<PackedZ3<81> > >& Rmat16,
                         std::vector<std::vector<PackedZ3<81> > >& lookup_table);
void uselookup(PackedZ3<81>& result_sum, std::vector<uint64_t>& outKX_input, std::vector<std::vector<PackedZ3<81> > >& lookup_table);

void usedLookupTable(PackedZ3<81>& outZ3, PackedZ3<256>& inVec, std::vector<std::vector<PackedZ3<81> > >& lookup_prf);

#endif //NEWPROTOCOL_LOOKUP_FUNCTIONS_H
