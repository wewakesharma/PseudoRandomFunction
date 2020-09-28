//
// Created by Vivek Sharma on 9/26/20.
//

#include <iostream>
#include <cstdlib>
#include "packedMod2.hpp"
#include "packedMod3.hpp"
#include "mains.hpp"
#include "PRF.hpp"
#include "OT.hpp"
#include "Timing.hpp"
#include "newprotocol.h"
#include "newprotocol_test.h"
#include <chrono>

void test_round3(std::vector<uint64_t>& K, PackedZ2<N_COLS>& X, PackedZ3<N_SIZE>& res1, PackedZ3<N_SIZE>& res2)
{
    PackedZ2<N_COLS> outKX;
    outKX.toeplitzByVec(K,X);

    //converting K*x into Z3 values
    std::vector<unsigned int> outKX_unsgn;//unsigned int of outKX i.e.(K*x)
    outKX.toArray(outKX_unsgn);

    PackedZ3<256> outKX_Z3;//packed Z3
    outKX_Z3.fromArray(outKX_unsgn);

    PackedZ3<N_SIZE> res;
    res = res1;
    res.add(res2);

    std::cout<<"outKX_Z3 "<<outKX_Z3<<std::endl;
    std::cout<<"res1 "<<res1<<std::endl;
    std::cout<<"res2 "<<res2<<std::endl;
    std::cout<<"res "<<res<<std::endl;
    if(outKX_Z3 == res)
        std::cout<<"Round 3: Test passed"<<std::endl;
    else
        std::cout<<"Round 3: Test fails"<<std::endl;
}

void test_round2(std::vector<uint64_t>& K, PackedZ2<N_COLS>& X,PackedZ2<N_COLS>& rw, PackedZ2<N_COLS>& w_mask)
{
    PackedZ2<N_COLS> outKX;
    outKX.toeplitzByVec(K,X);

    //converting K*x into Z3 values
    std::vector<unsigned int> outKX_unsgn;//unsigned int of outKX i.e.(K*x)
    outKX.toArray(outKX_unsgn);

    PackedZ3<256> outKX_Z3;//packed Z3
    outKX_Z3.fromArray(outKX_unsgn);

    PackedZ2<N_COLS> KXplusrw;
    KXplusrw = outKX;
    KXplusrw.add(rw);

    std::cout<<"test_unit_newprotocol.cpp/test_round2(): printing values"<<std::endl;
    std::cout<<"outKX "<<outKX<<std::endl;
    std::cout<<"KXplusrw "<<KXplusrw<<std::endl;
    std::cout<<"w_mask "<<w_mask<<std::endl;

    if(KXplusrw == w_mask)
        std::cout<<"Round 2: Test passed"<<std::endl;
    else
        std::cout<<"Round 2: Test fails"<<std::endl;

}

