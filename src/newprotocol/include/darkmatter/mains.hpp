#ifndef _EXECUTION_HEADER_H_
#define _EXECUTION_HEADER_H_

#define PRINT_VAL //used to print the intermediate values

//#define DEBUG //used to set bits of intermediate values

//#define UNIT_NP

// Uncomment one and only one
//#define TEST_PHASE1
//#define TEST_PackedMod2
//#define TEST_PackedMod3
//#define TEST_PRF
//#define TEST_SC
//#define PACKED_PRF_CENTRAL
//#define UNPACKED_PRF_CENTRAL
#define TEST_NP //runs the new protocol(full)
#define UNITTEST_ROUND2 //runs the unit test code that checks round 2 only
//#define TEST_DM


#endif // _EXECUTION_HEADER_H

#ifdef UNITTEST_ROUND2
#define TEST_NP
#endif

