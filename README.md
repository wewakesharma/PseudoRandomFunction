# Optimized Implementation of Pseudorandom Functions


### Introduction:
The repository contains optimized code written to implement weak PRF with parameters m, n and t.
Currently m = n = 256 and t = 81 in Z3.

### How to use it
Here are the instructions to replicate the benchmarking procedure:
1. Clone the repository ($ git clone https://github.com/vivek18394/PseudoRandomFunction)
2. Set the appropriate flag. Check section Flags
3. run the command $g++ -std=c++14 [-O3] -o <output_file> -I include/darkmatter/ src/*.cpp tests/*.cpp

### Screenshots:
![Fully Distributed with packing](https://github.com/vivek18394/PseudoRandomFunction/tree/master/Documents/screenshots/np_aws.png)
![Fully Distributed with packing and lookup table](https://github.com/vivek18394/PseudoRandomFunction/tree/master/Documents/screenshots/np_lookup_aws.png)
![OPRF with packing](https://github.com/vivek18394/PseudoRandomFunction/tree/master/Documents/screenshots/oprf_aws.png)
![OPRF with packing and lookup table](https://github.com/vivek18394/PseudoRandomFunction/tree/master/Documents/screenshots/oprf_lookup_aws.png)

### Languages Used: C, C++.

### Motivation:


### Flags

### Contributors
* [Tzipora Halevi](https://github.com/thalevi)
* [Mahimna Kelkar](https://github.com/mahimnakelkar)

### Task Lists:
- [x] Dark Matter TCC '18 wPRF 
- [x] Dark Matter TCC '18 wPRF with packing
- [] Dark Matter TCC '18 wPRF with packing and lookup table
- [x] Fully Distributed wPRF
- [x] Fully Distributed wPRF with packing
- [x] Fully Distributed wPRF with packing and lookup table
- [x] Proposed OPRF with packing
- [x] Proposed OPRF with packing and lookup table
