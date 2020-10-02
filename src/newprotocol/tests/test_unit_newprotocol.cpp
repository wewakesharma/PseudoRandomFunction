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

/*
 *  runs unit 2 and test
 *
 *
 */
/*
void test_round2_unit(std::vector<uint64_t>& K1, PackedZ2<N_COLS>& x1,
                          std::vector<uint64_t>& K2, PackedZ2<N_COLS>& x2,
                          int nTime)
{
    std::vector<uint64_t> rK1(toeplitzWords), rK2(toeplitzWords), rK(toeplitzWords);
    PackedZ2<N_COLS> rx1, rx2, rx; //mask for input
    PackedZ2<N_COLS> sw1, sw2, sw;
    PackedZ2<N_COLS> rw1, rw2, rw;//rw = rK * rx + sw

    //declare a variable in Z3 called r0z and r1z
    PackedZ3<N_SIZE> r0z, r1z;
    PackedZ3<N_SIZE> r0z1, r0z2, r1z1, r1z2;

    PackedZ3<81> y_out_z3;//output of each parties

    //============================

    //1. Perform preprocessing
    preProc_mod2_dm2020(nTimes);//it was nRuns * 2 in previous protocol, needs to be changed later
    preProc_mod3_dm2020(nTimes);

    fetchPreproc_party1(rx1,rw1,sw1,rK1,r0z1,r1z1);
    fetchPreproc_party2(rx2,rw2,sw2,rK2, r0z2,r1z2);

#ifdef DEBUG
    std::cout<<"in PRF_new_protocol, rx1= "<<rx1<<std::endl;
    std::cout<<"PRF_new_protocol, rx2 "<<rx2<<std::endl;
#endif

    //3. Parties locally compute [x'] = [x] + [rx] and [K'] = [K] + [rk]
    PackedZ2<N_COLS> x1_mask, x_mask, x2_mask;
    std::vector<uint64_t> K1_mask(toeplitzWords), K2_mask(toeplitzWords), K_mask(toeplitzWords);

    party1_round_1(x1_mask,K1_mask,x1,rx1,K1,rK1);
    party2_round_1(x2_mask,K2_mask,x2,rx2,K2,rK2);

    //std::cout<<""<<std::endl;
    //both the parties are supposed to exchange their mask values
    compute_input_mask(x_mask, K_mask, x1_mask, x2_mask, K1_mask, K2_mask);

#ifdef DEBUG

    std::cout<<"newprotocol.cpp/PRF_new_protocol_central(): Round 1 ends"<<std::endl;
#endif

#ifdef DEBUG
    std::cout<<"newprotocol.cpp/PRF_new_protocol_central(): Round 2 begins"<<std::endl;
#endif

    //Round 2: Both parties compute w' = [k'x'] - k'[rx] - rk.x' + [rk * rk + rw]
    PackedZ2<N_COLS> w1_mask, w2_mask, w_mask; //w' = K'(x' - rx) - rK'*x' + sw
    party1_round2(w1_mask, K_mask,x_mask, rx1, rK1, sw1);
    party2_round2(w2_mask, K_mask,x_mask, rx2, rK2, sw2);

    compute_wmask(w_mask, w1_mask, w2_mask);

    PackedZ2<N_COLS> rw;

    test_round2(K, X, rw, w_mask);
}
*/

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

