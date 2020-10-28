// Copyright (C) 2020 Tzipora Halevi, MIT license - see LICENSE file
/** test_packedMod2.cpp
 *  - testing packed vectors in Z_2
 */
#include <iostream>
#include "packedMod2.hpp"

// bool isZero()
static bool test_isZero() {
    PackedZ2<150> v;
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
    PackedZ2<70> v;
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
    PackedZ2<70> v;
    const std::vector<unsigned int> numbers = {1,2,3,4,5,6,7,8,9,0,4,30,20,11,18,29,1,5,7,2,8,7,5,99,12,9,3,6,72,11,10,9,8,7,6,5,4,3,2,1,0,0xf000,2,0x80000000,(unsigned int)-1,5,4,3,2,1,0,9,8,7,6,5,4,3,2,1,100,101,102,103,110,123,456,789,1011,1};
    v.fromArray(numbers);

    for (int i=0; i<numbers.size(); i++) {
        if ((numbers[i] % 2) != v.at(i)) {
            std::cerr << "expected v.at("<<i<<")="<<numbers[i]<<"%2="
              <<(numbers[i]%2)<<" but found "<<v.at(i)<<std::endl;
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
    PackedZ2<70> v;
    v.set(0,1);
    v.set(1,98);
    v.set(63,2);
    v.set(64,1);
    v.set(65,0);
    v.set(69,101);
    const std::vector<unsigned int> expected 
      = {1,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0,
         0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0,
         0,0,0,0,1, 0,0,0,0,1};
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
    PackedZ2<5> v;
    const std::vector<unsigned int> numbers = {1,2,3,4,5};
    v.fromArray(numbers);
    for (int i=0; i<5; i++) {
        if (((i+1) % 2) != v.at(i)) {
            std::cerr << "expected v.at("<<i<<")="<<numbers[i]<<"%2="
              <<(numbers[i]%2)<<" but found "<<v.at(i)<<std::endl;
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
    PackedZ2<69> v;
    const std::vector<unsigned int> numbers = {1,2,3,4,5,6,7,8,9,0,4,30,20,11,18,29,1,5,7,2,8,7,5,99,12,9,3,6,72,11,10,9,8,7,6,5,4,3,2,1,0,0xf000,2,0x80000000,(unsigned int)-1,5,4,3,2,1,0,9,8,7,6,5,4,3,2,1,100,101,102,103,110,456,789,1011,1};
    v.fromArray(numbers);
    std::vector<unsigned int> obtained;
    v.toArray(obtained);
    for (int i=0; i<numbers.size(); i++) {
        if ((numbers[i] % 2) != obtained[i]) {
            std::cerr << "test_toArray: expected v.at("<<i<<")="
              <<numbers[i]<<"%2="<<(numbers[i]%2)
              <<" but found "<<obtained[i]<<std::endl;
              return false;
        }
    }
    return true;
}

// void makeFromBits(const std::vector<uint64_t>& hi,
//                   const std::vector<uint64_t>& lo)
static bool test_makeFromBits() {
    const std::vector<uint64_t> bits = { 0x0123456789abcdef, 0xdb97 };
    // bits is interpreted in big-endian ordering
    // bits=1111 0111 1011 0011 1101 0101 1001 0001
    //      1110 0110 1010 0010 1100 0100 1000 0000
    //      1110 1001 1011 1101 0000...
    const std::vector<unsigned int> expected
          = {1,1,1,1, 0,1,1,1, 1,0,1,1, 0,0,1,1,
                 1,1,0,1, 0,1,0,1, 1,0,0,1, 0,0,0,1,
             1,1,1,0, 0,1,1,0, 1,0,1,0, 0,0,1,0,
                 1,1,0,0, 0,1,0,0, 1,0,0,0, 0,0,0,0,
             1,1,1,0, 1};

    PackedZ2<69> v;
    v.makeFromBits(bits);
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
    PackedZ2<15> v;
    v.randomize(); // prg initiazalized by default
    const std::vector<unsigned int>
          expected = {0,1,1,0,1,1,1,1,1,0,0,1,0,1,0};
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

// void mux(const PackedZ2& other, const std::vector<uint64_t>& select)
static bool test_mux() {
    PackedZ2<64> u,v;
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


// void negate()
// PackedZ2 operator~() const
static bool test_negate() {
    PackedZ2<15> v;
    const std::vector<unsigned int>
          original = {0,0,1,0,0,0,1,1,0,0,1,0,0,1,1};
    v.fromArray(original);

    std::vector<unsigned int> negated;
    (~v).toArray(negated);

    const std::vector<unsigned int>
          expected = {1,1,0,1,1,1,0,0,1,1,0,1,1,0,0};
    if (negated != expected) {
        std::cerr << "test_negate obtained\n  "<<negated
                  << "\nbut expected\n  "<<expected<< std::endl;
        return false;
    }
    return true;
}

// void add(const PackedZ2& other)
// PackedZ2& operator+=(const PackedZ2& other)
static bool test_add() {
    PackedZ2<129> u,v;
    u.randomize();
    v.randomize();

    std::vector<unsigned int> x,y,z;    
    u.toArray(x);
    v.toArray(y);
    u ^= v;
    u.toArray(z);

    for (int i=0; i<x.size(); i++) {
        if ((x[i]+y[i])%2 != z[i]) {
            std::cerr << "test_add: x["<<i<<"]="<<x[i]
                  << ", y["<<i<<"]="<<y[i]
                  << " but (x+y)["<<i<<"]="<<z[i]<<std::endl;
            return false;
        }
    }
    return true;
}

// void multiplyBy(const PackedZ2& other)
// PackedZ2& operator*=(const PackedZ2& other)
static bool test_multiplyBy() {
    PackedZ2<29> u,v;
    u.randomize();
    v.randomize();

    std::vector<unsigned int> x,y,z;    
    u.toArray(x);
    v.toArray(y);
    u *= v;
    u.toArray(z);

    for (int i=0; i<x.size(); i++) {
        if ((x[i]*y[i])%2 != z[i]) {
            std::cerr << "test_multiplyBy: x["<<i<<"]="<<x[i]
                  << ", y["<<i<<"]="<<y[i]
                  << " but (x*y)["<<i<<"]="<<z[i]<<std::endl;
            return false;
        }
    }
    return true;
}

// using PackedMatrixZ2 = std::vector< PackedZ2 >;
// void multiplyByMat(const PackedMatrixZ2& matrix)
static bool test_matByVec() {
    PackedZ2<256> vec;                    // 256-vector
    std::vector< PackedZ2<81> > mat(256); // 81x256 matrix
    PackedZ2<81> out;                     // 81-vector
    
    // fill with random Z2 elements
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
        if ((sum %2) != outNums[i]) {
            std::cerr << "test_matByVec: "<<i<<": "
                      << (sum %2)<<"!="<<outNums[i]<<std::endl;
            return false;
        }
    }
    return true;
}

//  static void
//  toeplitzMatrix(PackedMatrixZ2& out,
//                 const std::vector<uint64_t>& T, int nCols)
static bool test_toeplitzMatrix() {
    std::vector< PackedZ2<4> > mat(8); // 4x8 matrix

    // 4+8-1=11 bits to specify this matrix (in the low bits of T[0])
    std::vector<uint64_t> T = { 0x652 }; // binary 110 0101 0011
    // represents (in big endian) the matrix:
    //    0 1 0 0 1 0 1 0
    //    1 0 0 1 0 1 0 0
    //    0 0 1 0 1 0 0 1
    //    0 1 0 1 0 0 1 1
    // binary of T in reverse is left column and bottom row

    PackedZ2<4>::toeplitzMatrix(mat, T, 8); // expand Teoplitz matrix

    // check the result
    std::vector< std::vector<unsigned int> > expected = {
        {0, 1, 0, 0}, {1, 0, 0, 1}, {0, 0, 1, 0}, {0, 1, 0, 1},
        {1, 0, 1, 0}, {0, 1, 0, 0}, {1, 0, 0, 1}, {0, 0, 1, 1}
    };
    std::vector< std::vector<unsigned int> > obtained(8);
    for (unsigned int i=0; i<mat.size(); i++) {
        mat[i].toArray(obtained[i]);
    }
    if (obtained != expected) {
        std::cerr << "test_toeplitzMatrix: ";
        if (expected.size() != obtained.size())
            std::cerr << "expected #columns="<<expected.size()
                     << "but found "<<obtained.size() << std::endl;
        else for (unsigned int i=0; i<expected.size(); i++) {
            if (obtained[i] != expected[i])
                std::cerr << i<< ": expected\n  " << expected[i]
                << " but obtained\n  " << obtained[i] << std::endl;
        }
        return false;
    }
    return true;
}

// template<unsigned int COLS>
// void toeplitzByVec(const std::vector<uint64_t>& T,PackedZ2<COLS> &vec)
static bool test_toeplitzByVec() {
    PackedZ2<256> vec;                     // 256-vector
    std::vector< PackedZ2<192> > mat(256); // 192x256 matrix
    PackedZ2<192> out;                     // 192-vector

    // 256+192-1=457 bits to specify this matrix
    std::vector<uint64_t> T(7);
    for (unsigned int i=0; i<T.size(); i++)
        T[i] = randomWord();
    T[6] |= 0x8000000000000000; // turn on the MSB, it should be ignored

    out.toeplitzByVec(T, vec);  // out = T \times vec

    // check the result by copmuting explicitly the matrix and multiplying
    std::vector< PackedZ2<192> > mat2;
    PackedZ2<192>::toeplitzMatrix(mat2,T,256); // expand Teoplitz matrix

    PackedZ2<192> out2;
    out2.matByVec(mat2,vec); // compute matrix-by-vector multiply
    if (out != out2) {
        std::cerr << "test_toeplitzByVec: error" << std::endl;
        return false;
    }

    return true;
}

#ifdef TEST_PHASE2

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
  trials++; if (test_multiplyBy()) successes++;
  trials++; if (test_matByVec()) successes++;
  trials++; if (test_toeplitzMatrix()) successes++;
  trials++; if (test_toeplitzByVec()) successes++;

  if (successes==trials)
      std::cout << "all tests pass" << std::endl;
  else
      std::cout << successes<<" out of "<<trials<<" passed"<<std::endl;
}

#endif