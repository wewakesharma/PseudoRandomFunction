# Optimized Implementation of Pseudorandom Functions


### 1. Introduction:
The repository contains optimized code written to implement weak PRF with parameters m, n and t.
Currently m = n = 256 and t = 81 in Z3.

### 2. How to run the protocols
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
The repository houses a custom built framework and variations of 2-3 wPRF proposed in Dark Matter crypto paper (TCC '18). 

#### 5.1. Dark Matter wPRF
##### 5.1.1. Files
* Basics - include/darkmatter/{PackedMod2.hpp, PackedMod3.hpp, utils.hpp, Timing.hpp}
* src/Toeplitz-by-x.cpp
* src/OT.cpp
* src/PRF.cpp
* tests/test_PRF.cpp
##### 5.1.2 Phases
1. **First phase** contains the mod-2 multiplication of shares of input(x) and Key(K) in field &#8484;<sub>2</sub> and then convert it to &#8484;<sub>3</sub>. These operations are carried out in two sub phases:
	i) K*x mod 2
	In this phase, the computation is divided into online module and an offline module. Recall that, both party has share of input(x<sub>1</sub>, x<sub>2</sub>) and Key(K<sub>1</sub>, K<sub>2</sub>). The aim is to compute 
	K *x = (K<sub>1</sub>, K<sub>2</sub>) * (x<sub>1</sub>, x<sub>2</sub>) = K<sub>1</sub>x<sub>1</sub> + K<sub>1</sub>x<sub>2</sub> + K<sub>2</sub>x<sub>1</sub> + </sub>K<sub>2</sub>x<sub>1</sub>
	The first and last term can be computed locally since it is with the parties, while second and third term needs to be computed by exchanging messages without actually sharing the shares. This part comes under online computation.
	Refer to 5.1.3 for functions.
	ii) mod2 to mod 3 conversion


party1_local_computation


	
2. **Second Phase** 
##### 5.2.3. Functions

The related functions can be found in files listed in 5.2.1.
3. **Third Phase**

##### 5.1.3. Functions

The related functions can be found in files listed in 5.1.1.
Function|  Description
--------------|-------------
topelitz_Party2_1()| Computes M<sub>x</sub> = x - r<sub>x</sub>
topelitz_Party1()| Computes m<sub>A</sub> = A - r<sub>A</sub>  and m<sub>b</sub> = (r<sub>A</sub> * M<sub>x</sub>) + b + r<sub>b</sub>
topelitz_Party2_2()|Computes m<sub>A</sub> * x + m<sub>b</sub> - r<sub>z</sub> where r<sub>z</sub> = r<sub>A</sub> * r<sub>x</sub> + r<sub>b</sub>
	
#### 5.2. Fully Distributed wPRF(proposed)
##### 5.2.1. Files
* Basics - include/darkmatter/{PackedMod2.hpp, PackedMod3.hpp, utils.hpp, Timing.hpp}
* src/newprotocol.cpp
* tests/test_newprotocol.cpp
* 
#### 5.3. (2,3)-wPRF based OPRF 
##### 5.3.1. Files
* Basics - include/darkmatter/{PackedMod2.hpp, PackedMod3.hpp, utils.hpp, Timing.hpp}
* src/oprf.cpp
* tests/test_oprf.cpp

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
#define TEST_PRF | Runs the Dark matter(TCC '18) protocol without the lookup table
#define TEST_PRF_LOOKUP | Runs the Dark matter(TCC '18) protocol using a lookup table
#define TEST_NP  | Runs the proposed Fully Distributed protocol without the lookup table
#define TEST_NP_LOOKUP | Runs the proposed Fully Distributed protocol using a lookup table
#define TEST_OPRF | Runs 23-wPRF based OPRF protocol without the lookup table
#define TEST_OPRF_LOOKUP | Runs 23-wPRF based OPRF protocol using a lookup table

##### 7.2 Centralized Implementation
Flags in mains.hpp | Description
-------------------- | ------------------
#define PACKED_PRF_CENTRAL | Runs operations in wPRF using Packing and BitSlicing
#define PACKED_PRF_CENTRAL_LOOKUP | Runs operations in wPRF using Packing, BitSlicing and lookup table
#define UNPACKED_PRF_CENTRAL | Runs operations in wPRF using Packing and naive method for matrix vector multiplication
#define TEST_INTEGER_PACKING | Runs operations in wPRF using Packing and Z3 integer packing. 


##### 7.3 Unit Tests
Flags in mains.hpp | Description
-------------------- | ------------------
#define TEST_LOOKUP_INDEPENDENT | Test standalone Lookup table implementation
#define UNIT_NP | Test Fully Distributed 23-wPRF
#define UNIT_NP_LOOKUP| Test Fully Distributed 23-wPRF with a lookup table
#define TEST_PHASE1| Test Ax+B phase 1 of Dark Matter TCC '18 const 3.1 protocol.


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
- [] Fix DM unit test issue-reported 1/29/2021
- [] Fix PackedMod2 unit test issue-reported 1/29/2021

