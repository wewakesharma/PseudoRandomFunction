//
// Created by Vivek Sharma on 8/25/20.
//

#ifndef DARKMATTERCODE_UNPACKED_PRF_CENTRAL_H
#define DARKMATTERCODE_UNPACKED_PRF_CENTRAL_H

#include "packedMod2.hpp" // defines PackedZ2
#include "packedMod3.hpp" // defines PackedPairZ2 and PackedZ3
#include "Toeplitz-by-x.hpp" //defines N_COLS

#define N_SIZE 256

void PRF_unpacked_driver(int nRuns);
void display_time_unpacked(int nRuns);
#endif //DARKMATTERCODE_UNPACKED_PRF_CENTRAL_H
