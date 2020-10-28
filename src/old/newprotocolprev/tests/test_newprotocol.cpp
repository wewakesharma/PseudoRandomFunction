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
    //call the centralized version of the new protocol
    std::cout<<"test_newprotocol/main.cpp: test_newprotocol"<<std::endl;
    PRF_new_protocol_central();
    return 0;
}

#endif