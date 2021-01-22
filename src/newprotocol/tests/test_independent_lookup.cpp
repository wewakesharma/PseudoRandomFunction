//
// Created by Vivek Sharma on 1/21/21.
//
/*
 * This file is to investigate more on uselookup timing.
 */
//include all the header files(from PRF.cpp)
#include <iostream>
#include <cassert>
#include "Toeplitz-by-x.hpp"
#include "OT.hpp"
#include "mains.hpp"
#include "Timing.hpp"
#include <chrono>
#include "packedMod2.hpp"
#include "packedMod3.hpp"
#include "utils.hpp"
#include "lookup_functions.h"

#ifdef TEST_LOOKUP_INDEPENDENT

//main function
int main()
{
     //=========================test code for uselookup timing by calling uselookup from PRF.cpp========

    //step 0: variable to time the lookup
    long timer_test_lookup = 0;

    //Step 1: Create a randomize input(test_res)
    std::vector<uint64_t> test_res_msb(16) ;
    std::vector<uint64_t> test_res_lsb(16) ;

    //Declare and add randomize Rmat
    std::vector<PackedZ3<81> > test_Rmat(256);
    for (auto &col : test_Rmat) // iterate over the columns
        col.randomize();
    std::cout<<std::endl<<"Rmat Created"<<std::endl;

    //Reformat rmat from 81 X 256 into 16 X 81 X 16
    std::vector<std::vector<PackedZ3<81> > > test_Rmat16(16); //Rmat16_prf vector of size 16 X 16 X 81 (GLOBAL)
    reformat_Rmat(test_Rmat16, test_Rmat);
    std::cout<<std::endl<<"Rmat reformatted"<<std::endl;

    //create lookup tabble locally
    std::vector<std::vector<PackedZ3<81> > > test_lookup_table(16);//declaration of lookup table
    create_lookup_table(test_Rmat16, test_lookup_table);
    std::cout<<std::endl<<"Lookup table created"<<std::endl;

    //step 1a. Create a PackedZ3<256> variable and randomize it
    PackedZ3<256> test_z3;
    test_z3.randomize();

    //step1b. call reformat_input and reformat test_z3's lsb or msb into test_res
    reformat_input(test_res_msb,test_z3.msbs());
    reformat_input(test_res_lsb,test_z3.lsbs());

    std::cout<<std::endl<<"Input generated and reformatted"<<std::endl;

    //step 3. Output variable
    PackedZ3<81> test_out_msb, test_out_lsb, test_out, test_dummy;

    //step 3a. call uselookup with test_res ,test_lookup and test_out for 1000 times
    std::chrono::time_point<std::chrono::system_clock> start_test_lookup;
    start_test_lookup = std::chrono::system_clock::now();

    for(int runs = 0; runs < 1000; runs++)
    {
        uselookup(test_out_msb, test_res_msb, test_lookup_table);
        uselookup(test_out_lsb, test_res_lsb, test_lookup_table);

        test_out = test_out_lsb;
        test_out.subtract(test_out_msb);

        test_dummy += test_out;
    }
    timer_test_lookup = (std::chrono::system_clock::now() - start_test_lookup).count();

    std::cout<<"Final random output"<<test_out<<std::endl;
    //display the timing
    using Clock = std::chrono::system_clock;
    using Duration = Clock::duration;
    //std::cout << Duration::period::num << " , " << Duration::period::den << '\n';
    float time_unit_multiplier = 1;
    if(Duration::period::den == 1000000000)
        time_unit_multiplier = 0.001; //make nanosecond to microsecond
    else if(Duration::period::den == 1000000)
        time_unit_multiplier = 1;   //keep the unit as microsecond
    std::cout<<std::endl<<"Time for 1000 runs of test lookup is "<<(timer_test_lookup*time_unit_multiplier)<<std::endl;
    return 0;
}
//create a random packed Z3 element



#endif