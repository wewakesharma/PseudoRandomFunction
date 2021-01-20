#ifndef _EXECUTION_HEADER_H_
#define _EXECUTION_HEADER_H_

// Uncomment one and only one

/*
 * Centralized implementation flags
 */
//#define PACKED_PRF_CENTRAL //packed centralized PRF with matbyvec multiplication
//#define PACKED_PRF_CENTRAL_LOOKUP //packed centralized PRF with lookup table
//#define UNPACKED_PRF_CENTRAL  //unpacked centralized PRF, everything performed naively
//#define TEST_INTEGER_PACKING
/*
 * Decentralized implementation flags
 */

//#define DEBUG

//#define TEST_PRF              //runs the dark matter protocol
#define TEST_PRF_LOOKUP         //runs the dark matter with lookup table
//#define TEST_NP                 //runs the new protocol using MATBYVEC
//#define TEST_NP_LOOKUP        //runs the new protocol using LOOKUP TABLE(only difference is in round 3 compared to new protocol)
//#define TEST_OPRF               //runs the packed distributed 2-3OPRF using matByVec()
//#define TEST_OPRF_LOOKUP    //runs the OPRF protococl using LOOKUP table(only difference is in round 3 compared to OPRF


/*
 * UNIT TEST FLAGS
 */
//#define UNIT_NP    //testing distributed new protocol, no lookup table
//#define UNIT_NP_LOOKUP    //testing distributed new protocol with lookup table
//#define UNIT_DM     //test the distributed dark matter protocol, no lookup table PROBLEM!

//#define UNIT_LOOKUP           //tests the lookup table implementation

//#define TEST_PHASE1         //test AX+B class
//#define TEST_PackedMod2    //test class packedMod2
//#define TEST_PackedMod3    //test class packedMod3
//#define TEST_SC     //Not used anymore

//#define UNITTEST_ROUND2       //for dark matter protocol - runs the unit test code that checks round 2 only


 /* Options:

PACKED_PRF_CENTRAL = 1.   - centralized packed PRF, both phases 2 and 3 are packed (no lookup table), key is Toeplitz:

UNPACKED_PRF_CENTRALIZED = 1. - Centralized naïve version unpacked

PACKED_PRF_CENTRAL_LOOKUP = 1 , centralized using lookup table

TEST_PRF= 1, Distributed dark matter version, packed, no lookup table

TEST_NP = 1, New protocol, packed, no lookup table

TEST_NP_LOOKUP = 1, New protocol, packed, no lookup table
 */




/*
 * DEBUGGING OPTIONS
 */
//#define PRINT_VAL //used to print the intermediate values
//#define DEBUG //used to set bits of intermediate values and print the values in new protocol
//#define OPRF_PRINT_VAL    //enable to print intermediate values in 23OPRF


#endif // _EXECUTION_HEADER_H

//===========================DO NOT TOUCH the following code: TEST_NP_LOOKUP REQUIRES TEST_NP==============

#ifdef TEST_NP_LOOKUP
#define TEST_NP
#endif

#ifdef TEST_PRF_LOOKUP
#define TEST_PRF
#endif

#ifdef UNIT_NP_LOOKUP
#define TEST_NP_LOOKUP
#endif

#ifdef TEST_OPRF_LOOKUP
#define TEST_OPRF
#endif

#ifdef UNITTEST_ROUND2
#define TEST_NP
#endif

//======================================================================================
