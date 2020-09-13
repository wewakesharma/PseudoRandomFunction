#ifndef DARKMATTER_PACKED2_H
#define DARKMATTER_PACKED2_H
// Copyright (C) 2020 Tzipora Halevi, MIT license - see LICENSE file
/** test_packedMod2.hpp
 *  - implemntation of packed vectors in Z_2
 */

#include <cstdint>
#include <vector>
#include <iostream>
#include <exception>
#include "utils.hpp"

/**
 * PackedZ2 - a packed vector of Z_2 elements
 *
 * represented as two vectors for the msb's and lsb's.
 **/
template <size_t SIZE>
class PackedZ2 {
public:
    std::vector<uint64_t> bits;

    // how many 64-bit words to keep them all
    static const size_t nWords = (SIZE+63)/64;

    size_t size() const { return SIZE; }
    size_t wSize() const { return nWords; }

private: // some utility functions
    // zero out the unused portion of the last word (if any)
    void zeroExtraBits() {
        int extraBits = 64*nWords - SIZE;
        if (extraBits) {
            uint64_t mask = 0x8000000000000000; // 1 at the top bit
            mask >>= (extraBits-1); // 1 at position 63-extraBits+1
            mask = -mask;     // 1 at positions 63...63-extraBits+1
            mask = ~mask;     // 1 at positions 63-extraBits...0
            bits[nWords-1] &= mask;
        }
    }

public:
    // set to all-zero
    void reset() { bits.assign(nWords,0); }

    // Constructor
    PackedZ2() { reset(); }

    // get the element at position idx
    int at(int idx) const {
        if (idx >= SIZE) {
            throw std::out_of_range("Trying to get index "
                +std::to_string(idx)
                +" but size="+std::to_string(SIZE));
        }
        int wIdx = idx >> 6;        // divide by 64
        int idxInWord = idx & 0x3f; // modulo 64

        return int(bits[wIdx] >> idxInWord) & 1;
    }

    // set the element at position idx to val mod 2
    void set(int idx, unsigned int val) {
        if (idx >= SIZE) {
            throw std::out_of_range("Trying to set index "
                +std::to_string(idx)
                +" but size="+std::to_string(SIZE));
        }
        int wIdx = idx >> 6;        // divide by 64
        int idxInWord = idx & 0x3f; // modulo 64
        uint64_t mask = ~(uint64_t(1) << idxInWord);
        uint64_t b = uint64_t(val&1) << idxInWord;
        bits[wIdx] = (bits[wIdx] & mask) | b;
    }

    bool isZero() const {
        for (int wIdx=0; wIdx<nWords; wIdx++) {
            if (bits[wIdx] != 0)
                return false;
        }
        return true;
    }

    bool operator==(const PackedZ2& other) const {
        return bits==other.bits;
    }
    bool operator!=(const PackedZ2& other) const {
        return bits!=other.bits;
    }

    // initialize from an array of ints
    void fromArray(const std::vector<unsigned int>& src) {
        if (src.size() != SIZE) {
            throw std::logic_error("Trying to init from vector of length "
                +std::to_string(src.size())
                +" but packed size="+std::to_string(SIZE));
        }
        int i=0;
        for (int wIdx=0; wIdx<nWords; wIdx++) {
            bits[wIdx] = 0;
            for (int idxInWord=0; idxInWord<64; idxInWord++,i++) {
                if (i >= SIZE) break;
                bits[wIdx] |= uint64_t(src[i] & 1) << idxInWord;
            }
        }
    }

    //====(SAME AS ABOVE FUNCTION BUT WITH UINT64_T======Temporary Testing purpose=========
    void fromArray_64(const std::vector<uint64_t>& src) {
        if (src.size() != SIZE) {
            throw std::logic_error("Trying to init from vector of length "
                                   +std::to_string(src.size())
                                   +" but packed size="+std::to_string(SIZE));
        }
        int i=0;
        for (int wIdx=0; wIdx<nWords; wIdx++) {
            bits[wIdx] = 0;
            for (int idxInWord=0; idxInWord<64; idxInWord++,i++) {
                if (i >= SIZE) break;
                bits[wIdx] |= uint64_t(src[i] & 1) << idxInWord;
            }
        }
    }

    void toArray_64(std::vector<uint64_t>& dst) const {
        dst.resize(SIZE); // set the size
        int i=0;
        for (int wIdx=0; wIdx<nWords; wIdx++) {
            for (int idxInWord=0; idxInWord<64; idxInWord++,i++) {
                if (i >= SIZE) break;
                dst[i] = (bits[wIdx] >> idxInWord) & 1;
            }
        }
    }
    //==================

    // dump content to an array of ints
    void toArray(std::vector<unsigned int>& dst) const {
        dst.resize(SIZE); // set the size
        int i=0;
        for (int wIdx=0; wIdx<nWords; wIdx++) {
            for (int idxInWord=0; idxInWord<64; idxInWord++,i++) {
                if (i >= SIZE) break;
                dst[i] = (bits[wIdx] >> idxInWord) & 1;
            }
        }
    }

    // Initialize from s bit-array
    void makeFromBits(const std::vector<uint64_t>& bs) {
        if (bs.size() != nWords) {
            throw std::logic_error("Init from bit-arrays of size "
                +std::to_string(bs.size())
                +" but packed size="+std::to_string(SIZE));
        }
        bits = bs;
        zeroExtraBits(); // zero out unused portion of last word (if any)
    }
    PackedZ2& operator=(const std::vector<uint64_t>& bs) {
        makeFromBits(bs); return *this;
    }

    // fill with random(-enough) elements in {0,1,2}
    void randomize() {
        for (int wIdx=0; wIdx<nWords; wIdx++) {
            bits[wIdx] = randomWord();
        }
        zeroExtraBits(); // zero out unused portion of last word (if any)
    }

    // Bitwise negation
    void negate() {
        for (int wIdx=0; wIdx<nWords; wIdx++)
            bits[wIdx] = ~bits[wIdx];
        zeroExtraBits(); // zero out unused portion of last word (if any)
    }
    PackedZ2 operator~() const {
        PackedZ2 tmp = *this;
        tmp.negate();
        return tmp;
    }


    //This method cannot work since bits is a data member of class PackedZ2 and we are passing uint64_t vector
    /*void add_vec(const std::vector<uint64_t>& other){
        for (int wIdx=0; wIdx<nWords; wIdx++)
            bits[wIdx] ^= other.bits[wIdx];
    }*/
    // Addition
    void add(const PackedZ2& other) {
        for (int wIdx=0; wIdx<nWords; wIdx++)
            bits[wIdx] ^= other.bits[wIdx];
    }
    PackedZ2& operator^=(const PackedZ2& other) {
        add(other); return *this; 
    }
    
    // Multiplication
    void multiplyBy(const PackedZ2& other) {
        for (int wIdx=0; wIdx<nWords; wIdx++)
            bits[wIdx] &= other.bits[wIdx];
    }
    PackedZ2& operator*=(const PackedZ2& other) {
        multiplyBy(other); return *this; 
    }
    PackedZ2& operator&=(const PackedZ2& other) {
        multiplyBy(other); return *this;
    }
    // For each position i, returns this->at(i) if the i'th
    // bit in select is 0, or other.at(i) if the i'th bit is 1
    void mux(const PackedZ2& other, const std::vector<uint64_t>& select) {
        if (select.size() != nWords) {
            throw std::logic_error("select argument to mux has size "
                +std::to_string(select.size())+" but packed size="
                +std::to_string(SIZE));
        }
        for (int i=0; i<nWords; i++) {
            bits[i] = (other.bits[i] & select[i])
                       | (bits[i] & (~(select[i])));
        }
    }

    // Matrix-vector multiplication
    using PackedMatrixZ2 = std::vector< PackedZ2 >;
    template<size_t COLS>
    void matByVec(const PackedMatrixZ2& mat, PackedZ2<COLS> &vec) {
        if (mat.size() != COLS) {
            throw std::logic_error("Multiplying a "+std::to_string(SIZE)
                +"-by-"+std::to_string(mat.size())
                +" matrix by a packed "+std::to_string(COLS)
                +" vector");
        }
        reset(); // initialize accumulator to zero
        for (int cIdx=0; cIdx<COLS; cIdx++) {
 //         uint64_t bit = -vec.at(cIdx);   // all-0 or all-1
            int i = cIdx >> 6;    // divide by 64
            int j = cIdx & 0x3f;  // modulo 64
            uint64_t bit = (vec.bits[i] >> j) & 1;
            bit = -bit;           // all-0 or all-1

            // multiply the column by bit
            PackedZ2 col = mat[cIdx]; // a local copy
            for (int wIdx=0; wIdx<nWords; wIdx++) 
                col.bits[wIdx] &= bit;
            *this ^= col;
        }
    }

    // Expand a Topleitz matrix: T contains the 1st column and last
    // row of the matrix, in bit-endian. Namely, we are given the
    // vector T = (x_{1,1},x_{2,1},...,x_{n,1},x_{n,2},...,x_{n,m})
    // corresponding to the following places in the matrix:
    //   x_{1,1}
    //   x_{2,1}
    //   ...
    //   x_{n,1} x_{n,2} ... x_{m,n}
    // 
    static void
    toeplitzMatrix(PackedMatrixZ2& out,
                   const std::vector<uint64_t>& T, int nCols) {
        if (T.size() != (nCols+SIZE-1+63)/64) {
            throw std::logic_error("Expecting a "+std::to_string(SIZE)
                +"-by-"+std::to_string(nCols)
                +" Toeplitz but found "+std::to_string(T.size())
                +" 64-bit words");
        }
        out.resize(nCols);
        for (int cIdx=0; cIdx<nCols; cIdx++) { // one column at a time
            int i = cIdx >> 6;    // divide by 64
            int j = cIdx & 0x3f;  // modulo 64

            // expand column cIdx
            for (int wIdx=0; wIdx<nWords; wIdx++) {
                // next word of the cIdx column
                uint64_t colWord;
                if (j==0)
                  colWord = T[wIdx+i];
                else {
                  colWord = T[wIdx+i] >> j;             // low 64-j bits
                  if (wIdx+i+1 < T.size())
                      colWord |= T[wIdx+i+1] << (64-j); // high j bits
                }
                out[cIdx].bits[wIdx] = colWord;
            }
            // zero out unused portion of last word (if any)
            out[cIdx].zeroExtraBits();
        }
    }

    // Multiply a Toeplitz matrix by vector, the matrix is
    //represented as in the function toeplitzMatrix above
    template<size_t COLS>
    void toeplitzByVec(const std::vector<uint64_t>& T,PackedZ2<COLS> &vec){
        if (T.size() != (COLS+SIZE-1+63)/64) {
            throw std::logic_error("Expecting a "+std::to_string(SIZE)
                +"-by-"+std::to_string(COLS)
                +" Toeplitz but found "+std::to_string(T.size())
                +" 64-bit words");
        }
        reset(); // initialize accumulator to zero
        for (int cIdx=0; cIdx<COLS; cIdx++) { // one column at a time
 //         uint64_t bit = -vec.at(cIdx); // all-0 or all-1
            int i = cIdx >> 6;    // divide by 64
            int j = cIdx & 0x3f;  // modulo 64
            uint64_t bit = (vec.bits[i] >> j) & 1;
            bit = -bit;           // all-0 or all-1

            // add column cIdx*bit to the accumulator
            for (int wIdx=0; wIdx<nWords; wIdx++) {
                // next word of the cIdx column
                uint64_t colWord;
                if (j==0)
                  colWord = T[wIdx+i];
                else {
                  colWord = T[wIdx+i] >> j;             // low 64-j bits
                  if (wIdx+i+1 < T.size())
                      colWord |= T[wIdx+i+1] << (64-j); // high j bits
                }
                colWord &= bit;        // multiply by bit
                bits[wIdx] ^= colWord; // and add to accumulator
            }
        }
        zeroExtraBits(); // zero out unused portion of last word (if any)
    }

    friend std::ostream& operator<<(std::ostream& s, const PackedZ2& v) {
        std::vector<unsigned int> a;
        v.toArray(a);
        return s<<a;
    }
    friend std::istream& operator<<(std::istream& s, PackedZ2& v) {
        std::vector<unsigned int> a;
        s>>a;
        v.fromArray(a);
        return s;
    }
};

#endif // ifndef DARKMATTER_PACKED2_H
