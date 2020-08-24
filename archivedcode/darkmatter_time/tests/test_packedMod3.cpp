// Copyright (C) 2020 Tzipora Halevi, MIT license - see LICENSE file
/** test_packedMod3.cpp
 *  - testing packed vectors in Z_3
 */
#include <iostream>
#include "packedMod3.hpp"
//#include "mains.h"

// bool isZero()
static bool test_isZero() {
    PackedZ3<150> v;
    if (!v.isZero()) {
        std::cerr << "fresh vector is nonzero" << std::endl;
        return false;
    }
    v.randomize();
     if (v.isZero()) {
        std::cerr << "random vector is zero" << std::endl;
        return false;
    }
    return true;
}

// void reset()
static bool test_reset() {
    const std::vector<unsigned int> expected = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    PackedZ3<70> v;
    v.randomize();
    v.reset();
    std::vector<unsigned int> obtained;
    v.toArray(obtained);
    if (obtained != expected) {
        std::cerr << "reset returns a non-zero vector" << std::endl;
        return false;
    }
    return true;
}

// int at(int idx)
static bool test_at() {
    PackedZ3<70> v;
    const std::vector<unsigned int> numbers = {1,2,3,4,5,6,7,8,9,0,4,30,20,11,18,29,1,5,7,2,8,7,5,99,12,9,3,6,72,11,10,9,8,7,6,5,4,3,2,1,0,0xf000,2,0x80000000,(unsigned int)-1,5,4,3,2,1,0,9,8,7,6,5,4,3,2,1,100,101,102,103,110,123,456,789,1011,1};
    v.fromArray(numbers);

    for (int i=0; i<numbers.size(); i++) {
        if ((numbers[i] % 3) != v.at(i)) {
            std::cerr << "expected v.at("<<i<<")="<<numbers[i]<<"%3="
              <<(numbers[i]%3)<<" but found "<<v.at(i)<<std::endl;
              return false;
        }
    }
    bool exceptionTest = false;
    try {
        v.at(100);
    } catch(std::out_of_range& e) {
        exceptionTest = true;
    }
    if (!exceptionTest) {
        std::cerr << "v.at(100) did not raise an exception (v.SIZE=70)"
                  << std::endl;
        return false;
    }
    return true;
}

// void set(int idx, unsigned int val)
static bool test_set() {
    PackedZ3<70> v;
    v.set(0,1);
    v.set(1,98);
    v.set(63,2);
    v.set(64,1);
    v.set(65,0);
    v.set(69,100);
    const std::vector<unsigned int> expected 
      = {1,2,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0,
         0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0,
         0,0,0,2,1, 0,0,0,0,1};
    std::vector<unsigned int> obtained;
    v.toArray(obtained);
    if (obtained != expected) {
        std::cerr << "test_set obtained\n  "<<obtained
                  << "\nbut expected\n  "<<expected<< std::endl;
        return false;
    }

    bool exceptionTest = false;
    try {
        v.set(70,0);
    } catch(std::out_of_range& e) {
        exceptionTest = true;
    }
    if (!exceptionTest) {
        std::cerr << "v.set(70,0) did not raise an exception (v.SIZE=70)"
                  << std::endl;
        return false;
    }

    return true;
}

// void fromArray(const std::vector<unsigned int>& src)
static bool test_fromArray() {
    PackedZ3<5> v;
    const std::vector<unsigned int> numbers = {1,2,3,4,5};
    v.fromArray(numbers);
    for (int i=0; i<5; i++) {
        if (((i+1) % 3) != v.at(i)) {
            std::cerr << "expected v.at("<<i<<")="<<numbers[i]<<"%3="
              <<(numbers[i]%3)<<" but found "<<v.at(i)<<std::endl;
              return false;
        }
    }
    bool exceptionTest = false;
    try {
        const std::vector<unsigned int> wrongSize = {1,2,3,4};
        v.fromArray(wrongSize);
    } catch(std::logic_error& e) {
        exceptionTest = true;
    }
    if (!exceptionTest) {
        std::cerr << "v.fromArray({1,2,3,4}) did not raise an exception (v.SIZE=5)" << std::endl;
        return false;
    }
    return true;
}

// void toArray(std::vector<unsigned int>& dst)
static bool test_toArray() {
    PackedZ3<69> v;
    const std::vector<unsigned int> numbers = {1,2,3,4,5,6,7,8,9,0,4,30,20,11,18,29,1,5,7,2,8,7,5,99,12,9,3,6,72,11,10,9,8,7,6,5,4,3,2,1,0,0xf000,2,0x80000000,(unsigned int)-1,5,4,3,2,1,0,9,8,7,6,5,4,3,2,1,100,101,102,103,110,456,789,1011,1};
    v.fromArray(numbers);
    std::vector<unsigned int> obtained;
    v.toArray(obtained);
    for (int i=0; i<numbers.size(); i++) {
        if ((numbers[i] % 3) != obtained[i]) {
            std::cerr << "test_toArray: expected v.at("<<i<<")="
              <<numbers[i]<<"%3="<<(numbers[i]%3)
              <<" but found "<<obtained[i]<<std::endl;
              return false;
        }
    }
    return true;
}

// void makeFromBits(const std::vector<uint64_t>& hi,
//                   const std::vector<uint64_t>& lo)
static bool test_makeFromBits() {
    const std::vector<uint64_t> hi = { 0x0123456789abcdef, 0xdb97 };
    const std::vector<uint64_t> lo = { 0x5fedcba987654321, 0x1234 };
    // hi,lo are interpreted in big-endian ordering
    // hi = 1111 0111 1011 0011 1101 0101 1001 0001
    //      1110 0110 1010 0010 1100 0100 1000 0000
    //      1110 1001 1011 1101 0000...
    // lo = 1000 0100 1100 0010 1010 0110 1110 0001
    //      1001 0101 1101 0011 1011 0111 1111 1010
    //      0010 1100 0100 1000 0000...
    const std::vector<unsigned int> expected
          = {0,2,2,2, 0,0,2,2, 0,1,2,2, 0,0,0,2,
              0,2,1,2, 0,0,1,2, 0,1,1,2, 0,0,0,0,
             0,2,2,1, 0,0,2,1, 0,1,2,1, 0,0,0,1,
              0,2,1,1, 0,0,1,1, 0,1,1,1, 1,0,1,0,
             2,2,0,0, 0};
             
    PackedZ3<69> v;
    v.makeFromBits(hi,lo);
    std::vector<unsigned int> obtained;
    v.toArray(obtained);
    if (obtained != expected) {
        std::cerr << "test_makeFromBits obtained\n  "<<obtained
                  << "\nbut expected\n  "<<expected<< std::endl;
        for (int i=0; i<obtained.size(); i++) {
            if (expected[i]!=obtained[i]) {
                std::cerr << "  "<<i<<": expected "<<expected[i]
                          << " but obtained "<<obtained[i]<<std::endl;
            }
        }
        return false;
    }
    return true;
}

// void randomize()
static bool test_randomize() {
    PackedZ3<15> v;
    v.randomize(); // prg initiazalized by default
    const std::vector<unsigned int>
          expected = {1,2,2,0,1,2,2,1,2,0,0,2,1,2,1};
    // expected will change when the implementation of randomWord is fixed

    std::vector<unsigned int> obtained;
    v.toArray(obtained);
    if (obtained != expected) {
        std::cerr << "test_randomize obtained\n  "<<obtained
                  << "\nbut expected\n  "<<expected<< std::endl;
        return false;
    }
    return true;
}

// void negate()
// PackedZ3& operator-()
static bool test_negate() {
    PackedZ3<81> v;
    std::vector<unsigned int> x,y;    
    v.randomize();
    v.toArray(x);
    PackedZ3<81> w = -v;
    w.toArray(y);
    for (int i=0; i<x.size(); i++) {
        if (x[i]==0 && y[i]==0) continue;
        if (x[i]+y[i]==3) continue;
        std::cerr << "test_negate: x["<<i<<"]="<<x[i]
                  << "but (-x)["<<i<<"]="<<y[i]<< std::endl;
        return false;
    }
    return true;
}

// void mux(const PackedZ3& other, const std::vector<uint64_t>& select)
static bool test_mux() {
    PackedZ3<64> u,v;
    u.randomize();
    v.randomize();

    std::vector<unsigned int> x,y,z;
    std::vector<uint64_t> select(1);
    select[0] = randomWord();

    u.toArray(x);
    v.toArray(y);
    u.mux(v, select);
    u.toArray(z);

    for (int i=0; i<64; i++) {
        int expected = ((select[0] >> i) & 1)? x[i] : y[i];
        if (expected != z[i]) {
            std::cerr << "test_mux: x["<<i<<"]="<<x[i]
                  << ", y["<<i<<"]="<<y[i]
                  << ", but mux("<<((select[0] >> i)&1)
                  << ",x,y)["<<i<<"]="<<z[i]<< std::endl;
            return false;
        }
    }
    return true;
}

// void add(const PackedZ3& other)
// PackedZ3& operator+=(const PackedZ3& other)
static bool test_add() {
    PackedZ3<129> u,v;
    u.randomize();
    v.randomize();

    std::vector<unsigned int> x,y,z;    
    u.toArray(x);
    v.toArray(y);
    u += v;
    u.toArray(z);

    for (int i=0; i<x.size(); i++) {
        if ((x[i]+y[i])%3 != z[i]) {
            std::cerr << "test_add: x["<<i<<"]="<<x[i]
                  << ", y["<<i<<"]="<<y[i]
                  << " but (x+y)["<<i<<"]="<<z[i]<<std::endl;
            return false;
        }
    }
    return true;
}

// void subtract(const PackedZ3& other)
// PackedZ3& operator-=(const PackedZ3& other)
static bool test_subtract() {
    PackedZ3<81> u,v;
    u.randomize();
    v.randomize();

    std::vector<unsigned int> x,y,z;    
    u.toArray(x);
    v.toArray(y);
    u -= v;
    u.toArray(z);

    for (int i=0; i<x.size(); i++) {
        if ((3+x[i]-y[i])%3 != z[i]) {
            std::cerr << "test_subtract: x["<<i<<"]="<<x[i]
                  << ", y["<<i<<"]="<<y[i]
                  << " but (x-y)["<<i<<"]="<<z[i]<<std::endl;
            return false;
        }
    }
    return true;
}

// void multiplyBy(const PackedZ3& other)
// PackedZ3& operator*=(const PackedZ3& other)
static bool test_multiplyBy() {
    PackedZ3<29> u,v;
    u.randomize();
    v.randomize();

    std::vector<unsigned int> x,y,z;    
    u.toArray(x);
    v.toArray(y);
    u *= v;
    u.toArray(z);

    for (int i=0; i<x.size(); i++) {
        if ((x[i]*y[i])%3 != z[i]) {
            std::cerr << "test_multiplyBy: x["<<i<<"]="<<x[i]
                  << ", y["<<i<<"]="<<y[i]
                  << " but (x*y)["<<i<<"]="<<z[i]<<std::endl;
            return false;
        }
    }
    return true;
}

// using PackedMatrixZ3 = std::vector< PackedZ3 >;
// void multiplyByMat(const PackedMatrixZ3& matrix)
static bool test_matByVec() {
    PackedZ3<256> vec;                    // 256-vector
    std::vector< PackedZ3<81> > mat(256); // 81x256 matrix
    PackedZ3<81> out;                     // 81-vector
    
    // fill with random Z3 elements
    vec.randomize();
    for (auto &col : mat) // iterate over the columns
        col.randomize();

    out.matByVec(mat, vec); // compute matrix-by-vector multiply

    // check the result
    std::vector<unsigned int> vecNums;
    std::vector< std::vector<unsigned int> > matNums(256);
    std::vector<unsigned int> outNums;
    
    vec.toArray(vecNums);
    for (int i=0; i<mat.size(); i++) // iterate over the columns
        mat[i].toArray(matNums[i]);
    out.toArray(outNums);
    
    for (int i=0; i<81; i++) {
        uint64_t sum = 0;
        for (int j=0; j<256; j++)
            sum += matNums[j][i]*vecNums[j];
        if ((sum %3) != outNums[i]) {
            std::cerr << "test_matByVec: "<<i<<": "
                      << (sum %3)<<"!="<<outNums[i]<<std::endl;
            return false;
        }
    }
    return true;
}


#ifdef TEST_PHASE3

int main() {
  randomWord(7); // initialize the PRG with seed=7
  int trials = 0, successes = 0;

  trials++; if (test_randomize()) successes++; // keep this as 1st test
  trials++; if (test_isZero()) successes++;
  trials++; if (test_reset()) successes++;
  trials++; if (test_at()) successes++;
  trials++; if (test_set()) successes++;
  trials++; if (test_fromArray()) successes++;
  trials++; if (test_toArray()) successes++;
  trials++; if (test_makeFromBits()) successes++;
  trials++; if (test_negate()) successes++;
  trials++; if (test_add()) successes++;
  trials++; if (test_subtract()) successes++;
  trials++; if (test_multiplyBy()) successes++;
  trials++; if (test_matByVec()) successes++;

  if (successes==trials)
      std::cout << "all tests pass" << std::endl;
  else
      std::cout << successes<<" out of "<<trials<<" passed"<<std::endl;
}

#endif