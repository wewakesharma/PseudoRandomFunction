# Optimized Implementation of Pseudorandom Functions


### 1. Introduction:
The repository contains optimized code written to implement weak PRF with parameters m, n and t.
Currently m = n = 256 and t = 81 in Z3.

### 2. How to use it
Here are the instructions to replicate the benchmarking procedure:
1. Clone the repository ($ git clone https://github.com/vivek18394/PseudoRandomFunction)
2. Set the appropriate flag. Check section Flags
3. run the command $g++ -std=c++14 [-O3] -o <output_file> -I include/darkmatter/ src/*.cpp tests/*.cpp

### 3. Screenshots:
![Fully Distributed with packing](https://github.com/vivek18394/PseudoRandomFunction/tree/master/Documents/screenshots/np_aws.png)
![Fully Distributed with packing and lookup table](https://github.com/vivek18394/PseudoRandomFunction/tree/master/Documents/screenshots/np_lookup_aws.png)
![OPRF with packing](https://github.com/vivek18394/PseudoRandomFunction/tree/master/Documents/screenshots/oprf_aws.png)
![OPRF with packing and lookup table](https://github.com/vivek18394/PseudoRandomFunction/tree/master/Documents/screenshots/oprf_lookup_aws.png)

### 4. Languages Used: C, C++.

### 5. Implemented Functionalities

### 6. Timings:
This section describes the part that was timed in each protocol. The protocols are divided into phases and each phase of the protocol are timed. Some phases can run independently, which means they don't need any extra communication **while** running. Note: They do need inputs(at the beginning) and produces output(at the end). Some phases run while communicating, in such cases, parties indulge in communication **during** the execution of phase.
Some protocols can be parallelized, which means the parties running the phases of the protocol can run concurrently and in such cases, the time that a phase takes is the __maximum of the timing among two parties running on same phase/round__

#### Dark Matter wPRF(TCC '18):

    Begin DM_protocol(){
    Toeplitz_party2_1();
    Toeplitz_party1();
    Toeplitz_party2_2();

	Toeplitz_party2_1();
	Toeplitz_party1();
	Toeplitz_party2_1();
	}


#### Fully Distributed wPRF:

#### OPRF:


### 7. Flags

##### 7.1 Protocols Flags


Flag in mains.hpp | Description
------------------------- | -----------------
#define TEST_PRF | Runs the Dark matter protocol without the lookup table
#define TEST_PRF_LOOKUP | Runs the Dark matter protocol without the lookup table
#define TEST_NP  | Runs the Dark matter protocol without the lookup table
#define TEST_NP_LOOKUP | Runs the Dark matter protocol without the lookup table
#define TEST_OPRF | Runs the Dark matter protocol without the lookup table
#define TEST_OPRF_LOOKUP | Runs the Dark matter protocol without the lookup table

##### 7.2 Centralized Implementation
Flags in mains.hpp | Description
-------------------- | ------------------
#define PACKED_PRF_CENTRAL | Runs operations in wPRF using Packing and BitSlicing
#define PACKED_PRF_CENTRAL_LOOKUP | Runs operations in wPRF using Packing, BitSlicing and lookup table
#define UNPACKED_PRF_CENTRAL | Runs operations in wPRF using Packing and naive method for matrix vector multiplication
#define TEST_INTEGER_PACKING | Runs operations in wPRF using Packing and Z3 integer packing. 


##### 7.3 Unit Tests
#define TEST_LOOKUP_INDEPENDENT  
  
/*  
 * UNIT TEST FLAGS *///#define UNIT_NP    //testing distributed new protocol, no lookup table  
//#define UNIT_NP_LOOKUP    //testing distributed new protocol with lookup table  
  
//#define UNIT_DM     //test the distributed dark matter protocol, no lookup table PROBLEM!  
  
//#define UNIT_LOOKUP           //tests the lookup table implementation  
  
//#define TEST_PHASE1         //test AX+B class  
//#define TEST_PackedMod2    //test class packedMod2, PROBLEM!!!  
//#define TEST_PackedMod3    //test class packedMod3  
//#define TEST_SC     //Not used anymore  
  
//#define UNITTEST_ROUND2

##### Debugging Flag

### 7. Contributors
* [Tzipora Halevi](https://github.com/thalevi)
* [Mahimna Kelkar](https://github.com/mahimnakelkar)

### 8. Task Lists:
- [x] Dark Matter TCC '18 wPRF 
- [x] Dark Matter TCC '18 wPRF with packing
- [x] Dark Matter TCC '18 wPRF with packing and lookup table
- [x] Fully Distributed wPRF
- [x] Fully Distributed wPRF with packing
- [x] Fully Distributed wPRF with packing and lookup table
- [x] Proposed OPRF with packing
- [x] Proposed OPRF with packing and lookup table

