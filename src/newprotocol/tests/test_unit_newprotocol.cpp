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
#include "newprotocol.hpp"
#include "newprotocol_test.hpp"
#include <chrono>


void test_round1(PackedZ2<N_COLS>& X,PackedZ2<N_COLS>& rx, PackedZ2<N_COLS>& x_mask)
{
    PackedZ2<N_COLS> X_rx;
    X_rx = X;
    X_rx ^= rx;
    std::cout<<"X "<<X<<std::endl;
    std::cout<<"rx "<<rx<<std::endl;
    std::cout<<"X_rx "<<X_rx<<std::endl;
    std::cout<<"x_mask "<<x_mask<<std::endl;
    if(X_rx == x_mask)
        std::cout<<"Round 1: Test passed"<<std::endl;
    else
        std::cout<<"Round 1: Test fails"<<std::endl;

}

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

void set_random_inputs(std::vector<uint64_t>& K1, PackedZ2<N_COLS>& x1, std::vector<uint64_t>& K2,
                       PackedZ2<N_COLS>& x2, std::vector< PackedZ3<81> >& Rmat)
{
    randomWord(1); // use seed=1
    for (auto &w : K1) w = randomWord();
    K1[K1.size() - 1] &= topelitzMask; // turn off extra bits at the end
    for (auto &w : K2) w = randomWord();
    K2[K2.size() - 1] &= topelitzMask; // turn off extra bits at the end

    x1.randomize();
    x2.randomize();
    //generate a 81 X 256 randomization matrix in Z3.
    for (auto &col : Rmat) // iterate over the columns
        col.randomize();
}

#ifdef UNIT_NP
int main()
{
    std::vector<uint64_t> K1(toeplitzWords), K2(toeplitzWords);//key shares of parties
    PackedZ2<N_COLS> x1, x2; //input shares of parties
    std::vector<PackedZ3<81> > Rmat(256);
    PackedZ3<81> y_out_z3, y1_z3, y2_z3, outZ3;//output of new protocol

#ifdef DEBUG
    std::cout<<"test_unit_newprotocol.cpp/main(): TEST_NP is enabled"<<std::endl;
#endif

    set_random_inputs(K1,x1, K2,x2, Rmat);

    //centralized implementation of PRF
    PRF_packed_centralized(K1,x1, K2,x2, Rmat,outZ3,1); //output is outZ3

    //new protocol implementation of new protocol.
    PRF_new_protocol(K1,x1,K2, x2, Rmat, y1_z3, y2_z3, 1);//output is y1_z3 and y2_z3
    y_out_z3 = y1_z3;
    y_out_z3.add(y2_z3);

    if(y_out_z3 == outZ3)
        std::cout<<"Distributed New Protocol Without Lookup Unit Test Status... PASS"<<std::endl;
    else
        std::cout<<"Distributed New Protocol Without Lookup Unit Test Status... FAIL"<<std::endl;

    std::cout<<"y_out_z3 "<<y_out_z3<<std::endl;
    std::cout<<"out_Z3 "<<outZ3<<std::endl;
}
#endif


#ifdef UNIT_NP_LOOKUP
int main()
{
    std::vector<uint64_t> K1(toeplitzWords), K2(toeplitzWords);//key shares of parties
    PackedZ2<N_COLS> x1, x2; //input shares of parties
    std::vector<PackedZ3<81> > Rmat(256);
    PackedZ3<81> y_out_z3, y1_z3, y2_z3, outZ3;//output of new protocol

#ifdef DEBUG
    std::cout<<"test_unit_newprotocol.cpp/main(): TEST_NP_LOOKUP is enabled"<<std::endl;
#endif

    set_random_inputs(K1,x1, K2,x2, Rmat);

    //centralized implementation of PRF
    PRF_packed_centralized(K1,x1, K2,x2, Rmat,outZ3,1); //output is outZ3

    //new protocol implementation of new protocol.
    PRF_new_protocol(K1,x1,K2, x2, Rmat, y1_z3, y2_z3, 1);//output is y1_z3 and y2_z3
    y_out_z3 = y1_z3;
    y_out_z3.add(y2_z3);

    if(y_out_z3 == outZ3)
        std::cout<<"Distributed New Protocol With Lookup Unit Test Status... PASS"<<std::endl;
    else
        std::cout<<"Distributed New Protocol With Lookup Unit Test Status... FAIL"<<std::endl;

    std::cout<<"y_out_z3 "<<y_out_z3<<std::endl;
    std::cout<<"out_Z3 "<<outZ3<<std::endl;
}
#endif


