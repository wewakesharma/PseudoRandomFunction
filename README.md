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
1. **First phase** contains the mod-2 multiplication of shares of input(x) and Key(K) in field &#8484;<sub>2</sub> .These operations are carried out in two sub phases:
	i) K*x mod 2
	In this phase, the computation is divided into online module and an offline module. Recall that, both party has share of input(x<sub>1</sub>, x<sub>2</sub>) and Key(K<sub>1</sub>, K<sub>2</sub>). The aim is to compute 
	K *x = (K<sub>1</sub>, K<sub>2</sub>) * (x<sub>1</sub>, x<sub>2</sub>) = K<sub>1</sub>x<sub>1</sub> + K<sub>1</sub>x<sub>2</sub> + K<sub>2</sub>x<sub>1</sub> + </sub>K<sub>2</sub>x<sub>2</sub>
	The first and last term can be computed locally since it is with the parties, while second and third term needs to be computed by exchanging messages without actually sharing the shares. This part comes under online computation.
	Refer to 5.1.3 for functions.

2. **Second Phase** 
In this phase, the output of phase 1 is converted from  &#8484;<sub>2</sub> to &#8484;<sub>3</sub>. The related functions and their descriptions is in Section 5.1.3.
##### 5.2.3. Functions
The related functions can be found in files listed in 5.1.1.

3. **Third Phase**
This is the final phase of the protocol. It performs matrix vector multiplication between publicly available Random Matrix of size (t X m)[81 X 256] and m-bit sized output of phase 2(m &#8712;   &#8484;<sub>3</sub>). There are two different implementation of the matrix vector multiplication. This can be implemented by enabling flag no. 1 for packed, bit sliced version and flag no. 2 for lookup table as shown in Section 7.1.

##### 5.1.3. Functions

The related functions can be found in files listed in 5.1.1.
Function|  Description
--------------|-------------
topelitz_Party2_1()| Computes M<sub>x</sub> = x - r<sub>x</sub>
topelitz_Party1()| Computes m<sub>A</sub> = A - r<sub>A</sub>  and m<sub>b</sub> = (r<sub>A</sub> * M<sub>x</sub>) + b + r<sub>b</sub>
topelitz_Party2_2()|Computes m<sub>A</sub> * x + m<sub>b</sub> - r<sub>z</sub> where r<sub>z</sub> = r<sub>A</sub> * r<sub>x</sub> + r<sub>b</sub>
{SC_Party2_1(), SC_Party1(), SC_party 2_2()} | Converts  &#8484;<sub>2</sub> to &#8484;<sub>3</sub>.
matByVec() | Performs matrix Vector multiplication and outputs a t-bit vector. Not so fast, good on memory.
usedLookupTable()| Implements matrix vector multiplication using a lookup table. Takes more memory but is fast.
	
#### 5.2. Fully Distributed wPRF(proposed)
##### 5.2.1. Files
* Basics - include/darkmatter/{PackedMod2.hpp, PackedMod3.hpp, utils.hpp, Timing.hpp}
* src/newprotocol.cpp
* tests/test_newprotocol.cpp

##### 5.2.2 Phases
1. **First phase: Masking the inputs**
2. **Second phase: Compute w'(Kx)**
3. **Third phase: Perform matrix vector multiplication**

#### 5.3. (2,3)-wPRF based OPRF 
##### 5.3.1. Files
* Basics - include/darkmatter/{PackedMod2.hpp, PackedMod3.hpp, utils.hpp, Timing.hpp}
* src/oprf.cpp
* tests/test_oprf.cpp
##### 5.3.2 Phases
Server holds the key while client holds the input. The project includes Single client and single server implementation. 
1. **First phase: Masking the inputs**
Server masks its input and sends it to client, while client masks its input and sends it to server. 
2. **Second phase: Compute w'(Kx)**
Server Computes 
4. **Third phase: Perform matrix vector multiplication**

#### 5.4  Centralized PRF Implementation
##### 5.4.1 Files
* src/packed_PRF_central.cpp
* tests/test_Packed_PRF.cpp

#### 5.5 Centralized PRF Implementation with Lookup Table
##### 5.5.1 Files:
* tests/test_packed_PRF_central_lookup.cpp 


### 6. Timing Methodology:
This section describes the part that was timed in each protocol. The protocols are divided into phases and each phase of the protocol are timed. Some phases can run independently, which means they don't need any extra communication **while** running. Note: They do need inputs(at the beginning) and produces output(at the end). Some phases run while communicating, in such cases, parties indulge in communication **during** the execution of phase.
Some protocols can be parallelized, which means the parties running the phases of the protocol can run concurrently and in such cases, the time that a phase takes is the __maximum of the timing among two parties running on same phase/round__

#### Dark Matter wPRF(TCC '18):
* Time phase 1: The following three function are called twice in a single instance, one for  K<sub>1</sub>x<sub>2</sub> and other for  K<sub>2</sub>x<sub>1</sub>
	* Toeplitz_party2_1();
	* Toeplitz_party1();
	* Toeplitz_party2_2();


#### Fully Distributed wPRF:

#### OPRF:


### 7. Flags

##### 7.1 Protocols Flags

Sr. No. | Flag in mains.hpp | Description
---|------------------------- | -----------------
1|#define TEST_PRF | Runs the Dark matter(TCC '18) protocol without the lookup table
2|#define TEST_PRF_LOOKUP | Runs the Dark matter(TCC '18) protocol using a lookup table
3|#define TEST_NP  | Runs the proposed Fully Distributed protocol without the lookup table
4|#define TEST_NP_LOOKUP | Runs the proposed Fully Distributed protocol using a lookup table
5|#define TEST_OPRF | Runs 23-wPRF based OPRF protocol without the lookup table
6|#define TEST_OPRF_LOOKUP | Runs 23-wPRF based OPRF protocol using a lookup table

##### 7.2 Centralized Implementation
Sr. No. | Flags in mains.hpp | Description
|------------|-------------------- | ------------------
1|#define PACKED_PRF_CENTRAL | Runs operations in wPRF using Packing and BitSlicing
2|#define PACKED_PRF_CENTRAL_LOOKUP | Runs operations in wPRF using Packing, BitSlicing and lookup table
3|#define UNPACKED_PRF_CENTRAL | Runs operations in wPRF using Packing and naive method for matrix vector multiplication
4|#define TEST_INTEGER_PACKING | Runs operations in wPRF using Packing and Z3 integer packing. 


##### 7.3 Unit Tests
Sr. No. | Flags in mains.hpp | Description
|--------|-------------------- | ------------------
1|#define TEST_LOOKUP_INDEPENDENT | Test standalone Lookup table implementation
2|#define UNIT_NP | Test Fully Distributed 23-wPRF
3|#define UNIT_NP_LOOKUP| Test Fully Distributed 23-wPRF with a lookup table
4|#define TEST_PHASE1| Test Ax+B phase 1 of Dark Matter TCC '18 const 3.1 protocol.


### 8. Contributors
* [Tzipora Halevi](https://github.com/thalevi)
* [Mahimna Kelkar](https://github.com/mahimnakelkar)

### 9. Task Lists:
- [x] Dark Matter TCC '18 wPRF 
- [x] Dark Matter TCC '18 wPRF with packing
- [x] Dark Matter TCC '18 wPRF with packing and lookup table
- [x] Fully Distributed wPRF
- [x] Fully Distributed wPRF with packing
- [x] Fully Distributed wPRF with packing and lookup table
- [x] Proposed OPRF with packing
- [x] Proposed OPRF with packing and lookup table
- [ ] Fix DM unit test issue-reported 1/29/2021
- [ ] Fix PackedMod2 unit test issue-reported 1/29/2021

