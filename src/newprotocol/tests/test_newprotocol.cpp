//
// Created by Vivek Sharma on 9/2/20.
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
#include <chrono>

using namespace std;

void newprotocol_test(std::vector<uint64_t>& K1, PackedZ2<N_COLS>& x1, std::vector<uint64_t>& K2,
                      PackedZ2<N_COLS>& x2, std::vector< PackedZ3<81> >& Rmat, PackedZ3<81>& y1_z3,
                      PackedZ3<81>& y2_z3, int nTimes)
{


#ifdef DEBUG
    std::cout<<"newprotocol.cpp/PRF_new_protocol_central(): Calling PRF unit test function"<<std::endl;
#endif

    //PRF_packed_test(K1,x1,K2,x2,Rmat,test_out1_z3,test_out2_z3,1);
    //PRF_packed_centralized_test(K1,x1,K2,x2,Rmat,test_out1_z3,test_out2_z3,1);
    PRF_packed_centralized_res_compare(K1,x1,K2,x2,Rmat,y1_z3,y2_z3,nTimes);

    /*
    PackedZ3<81>test_out_z3 = test_out1_z3;
    test_out_z3.add(test_out2_z3);//out = out1 + out2

    //comparing test_out_z3 with y_out_z3
    bool test_flag = 0;//no problem
    for(int test_cnt = 0; test_cnt < 81; test_cnt++)
    {
        if(test_out_z3.at(test_cnt) != y_out_z3.at(test_cnt))
        {
            std::cout<<"The problem is at index "<<test_cnt<<std::endl;
            test_flag = 1;//some problem
            break;
        }
    }
    if(test_flag == 0)
        std::cout<<std::endl<<"New protocol test: Test passed"<<std::endl;
    else
        std::cout<<std::endl<<"New protocol test: Test fails"<<std::endl;*/
}
