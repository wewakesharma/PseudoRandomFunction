//
// Created by Vivek Sharma on 9/2/20.
//

#include <iostream>
#include <cstdlib>
#include "packedMod2.hpp"
#include "packedMod3.hpp"
#include "mains.hpp"
//#include "PRF.hpp"
#include "OT.hpp"
#include "Timing.hpp"
#include "newprotocol.h"
#include <chrono>

using namespace std;

#ifdef TEST_NP
int main()
{
    #ifdef DEBUG
    std::cout<<"test_newprotocol/main.cpp: Protocol Execution begins"<<std::endl;
    #endif
    //call the centralized version of the new protocol
    PRF_new_protocol_central();
    return 0;
}
#endif

#ifdef TEST_NP_DIST

int main()
{
    #ifdef DEBUG
    std::cout<<"test_newprotocol/main.cpp: Distributed Protocol Execution begins"<<std::endl;
    #endif
    //call the centralized version of the new protocol
    PRF_new_protocol_distributed();
    return 0;
}

#endif