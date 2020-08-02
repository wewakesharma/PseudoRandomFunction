#ifndef DARKMATTER_PACKED3_H
#define DARKMATTER_PACKED3_H
// Copyright (C) 2020 Tzipora Halevi, MIT license - see LICENSE file
/** test_packedMod3.hpp
 *  - implemntation of packed vectors in Z_3
 */

#include <cstdint>
#include <vector>
#include <iostream>
#include <exception>
#include "utils.hpp"

/**
 * PackedZ3 - a packed vector of Z_3 elements
 *
 * represented as two vectors for the msb's and lsb's.
 **/
template <unsigned int SIZE>
class PackedZ3 {
public:
    std::vector<uint64_t> msbs, lsbs;

    // how many 64-bit words to keep them all
    static const unsigned int nWords = (SIZE+63)/64;

    unsigned int size() const { return SIZE; }
    unsigned int wSize() const { return nWords; }

private: // some utility functions
    // zero out the unused portion of the last word (if any)
    void zeroExtraBits() {
        int extraBits = 64*nWords - SIZE;
        if (extraBits) {
            uint64_t mask = 0x8000000000000000; // 1 at the top bit
            mask >>= (extraBits-1); // 1 at position 63-extraBits+1
            mask = -mask;     // 1 at positions 63...63-extraBits+1
            mask = ~mask;     // 1 at positions 63-extraBits...0
            lsbs[nWords-1] &= mask;
            msbs[nWords-1] &= mask;
        }
    }

public:
    // set to all-zero
    void reset() { msbs.assign(nWords,0); lsbs.assign(nWords,0);}

    // Constructor
    PackedZ3() { reset(); }

    // get the element at position idx
    int at(unsigned int idx) const {
        if (idx >= SIZE) {
            throw std::out_of_range("Trying to get index "
                +std::to_string(idx)
                +" but size="+std::to_string(SIZE));
        }
        int wIdx = idx >> 6;        // divide by 64
        int idxInWord = idx & 0x3f; // modulo 64

        uint64_t msb = (msbs[wIdx] >> idxInWord) & 1;
        uint64_t lsb = (lsbs[wIdx] >> idxInWord) & 1;
 
        return int((msb << 1) | lsb);
    }

    // set the element at position idx to val mod 3
    void set(unsigned int idx, unsigned int val) {
        if (idx >= SIZE) {
            throw std::out_of_range("Trying to set index "
                +std::to_string(idx)
                +" but size="+std::to_string(SIZE));
        }
        int wIdx = idx >> 6;        // divide by 64
        int idxInWord = idx & 0x3f; // modulo 64
        val %= 3; // make sure that val is in 0,1,2
        uint64_t mask = ~(uint64_t(1) << idxInWord);
        uint64_t lsb = (uint64_t(val) & 1) << idxInWord;
        uint64_t msb = ((uint64_t(val)& 2)>>1) << idxInWord;
        msbs[wIdx] = (msbs[wIdx] & mask) | msb;
        lsbs[wIdx] = (lsbs[wIdx] & mask) | lsb;
    }

    bool isZero() const {
        for (int wIdx=0; wIdx<nWords; wIdx++) {
            if (msbs[wIdx] != 0 || lsbs[wIdx] != 0)
                return false;
        }
        return true;
    }

    bool operator==(const PackedZ3& other) const {
        return (msbs==other.bits && lsbs==other.lsbs);
    }
    bool operator!=(const PackedZ3& other) const {
        return (msbs!=other.bits || lsbs!=other.lsbs);
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
            msbs[wIdx] = lsbs[wIdx] = 0;
            for (int idxInWord=0; idxInWord<64; idxInWord++,i++) {
                if (i >= SIZE) break;
                uint64_t val = src[i] % 3; // val is in 0,1,2
                uint64_t lsb = (val & 1) << idxInWord;
                uint64_t msb = ((val& 2)>>1) << idxInWord;
                msbs[wIdx] |= msb;
                lsbs[wIdx] |= lsb;
            }
        }
    }

    // dump content to an array of ints
    void toArray(std::vector<unsigned int>& dst) const {
        dst.resize(SIZE); // set the size
        int i=0;
        for (int wIdx=0; wIdx<nWords; wIdx++) {
            for (int idxInWord=0; idxInWord<64; idxInWord++,i++) {
                if (i >= SIZE) break;
                uint64_t lsb = (lsbs[wIdx] >> idxInWord) & 1;
                uint64_t msb = (msbs[wIdx] >> idxInWord) & 1;
                dst[i] = (msb << 1) | lsb;
            }
        }
    }

    // Initialize from two bit-arrays for MSBs and LSBs
    // If we get (1,1) at any position then we record (0,0) instead
    void makeFromBits(const std::vector<uint64_t>& hi,
                      const std::vector<uint64_t>& lo) {
        if (hi.size() != nWords || lo.size() != nWords) {
            throw std::logic_error("Init from bit-arrays of sizes ("
                +std::to_string(hi.size())+','+std::to_string(lo.size())
                +") but packed size="+std::to_string(SIZE));
        }
        for (int wIdx=0; wIdx<nWords; wIdx++) {
            uint64_t lessThan3 = ~(lo[wIdx] & hi[wIdx]);
            msbs[wIdx] = hi[wIdx] & lessThan3;
            lsbs[wIdx] = lo[wIdx] & lessThan3;
            // 1,1 is recorded as 0,0
            // FIXME: issue some warning if found a (1,1)
        }
        zeroExtraBits(); // zero out unused portion of last word (if any)
    }

    // fill with random(-enough) elements in {0,1,2}
    void randomize() {
        for (int wIdx=0; wIdx<nWords; wIdx++) {
            uint64_t hi=0, lo=0;
            uint64_t done = 0;
            for (int j=0; j<100; j++) { // try at most 100 times
              uint64_t msb = randomWord();
              uint64_t lsb = randomWord();
              uint64_t valid= ~(msb & lsb);// ignore bit position with 1,1
              msb &= valid & (~done); // whether to use these bits
              lsb &= valid & (~done);
              hi |= msb;
              lo |= lsb;
              done |= valid;
              if (done == ~uint64_t(0)) break; // we're done
            }
            msbs[wIdx] = hi;
            lsbs[wIdx] = lo;
        }
        zeroExtraBits(); // zero out unused portion of last word (if any)
    }

    // Negation: if <m,l> \in Z_3 then <l,m> = -<m,l> (mod 3)
    void negate() { msbs.swap(lsbs); };
    PackedZ3& operator-() { negate(); return *this; }

    // Addition
    void add(const PackedZ3& other) {
        for (int wIdx=0; wIdx<nWords; wIdx++) {

            uint64_t m1 = msbs[wIdx];
            uint64_t l1 = lsbs[wIdx];
            uint64_t m2 = other.msbs[wIdx];
            uint64_t l2 = other.lsbs[wIdx];

            uint64_t T = (l1 | m2) ^ (l2 | m1);
            msbs[wIdx] = (l1 | l2 ) ^ T;
            lsbs[wIdx] = (m1 | m2 ) ^ T;

/*            uint64_t z1 = (~m1)&(~l1); // this==0
            uint64_t z2 = (~m2)&(~l2); // other==0
            lsbs[wIdx] = (m1&m2) | (l1&z2) | (z1&l2);
            msbs[wIdx] = (l1&l2) | (m1&z2) | (z1&m2);*/
        }
    }
    PackedZ3& operator+=(const PackedZ3& other) {
        add(other); return *this; 
    }
    
    // Subtraction
    void subtract(const PackedZ3& other) {
        for (int wIdx=0; wIdx<nWords; wIdx++) {

            uint64_t m1 = msbs[wIdx];
            uint64_t l1 = lsbs[wIdx];
            uint64_t m2 = other.lsbs[wIdx];
            uint64_t l2 = other.msbs[wIdx];

            uint64_t T = (l1 | m2) ^ (l2 | m1);
            msbs[wIdx] = (l1 | l2 ) ^ T;
            lsbs[wIdx] = (m1 | m2 ) ^ T;


/*            uint64_t m1 = msbs[wIdx];
            uint64_t l1 = lsbs[wIdx];
            uint64_t l2 = other.msbs[wIdx];
            uint64_t m2 = other.lsbs[wIdx];
            uint64_t z1 = (~m1)&(~l1); // this==0
            uint64_t z2 = (~m2)&(~l2); // other==0
            lsbs[wIdx] = (m1&m2) | (l1&z2) | (z1&l2);
            msbs[wIdx] = (l1&l2) | (m1&z2) | (z1&m2);*/
        }
    }
    PackedZ3& operator-=(const PackedZ3& other) {
        subtract(other); return *this; 
    }

    // Multiplication
    void multiplyBy(const PackedZ3& other) {
        for (int wIdx=0; wIdx<nWords; wIdx++) {
            // msb1,lsb1=(0,0) or msb2,lsb2=(0,0) => msb,lsb=(0,0)
            // msb1,lsb1=(0,1)  & msb2,lsb2=(0,1) => msb,lsb=(0,1)
            // msb1,lsb1=(1,0)  & msb2,lsb2=(1,0) => msb,lsb=(0,1)
            // msb1,lsb1=(0,1)  & msb2,lsb2=(1,0) => msb,lsb=(1,0)
            uint64_t lo = (lsbs[wIdx] & other.lsbs[wIdx])
                          ^ (msbs[wIdx] & other.msbs[wIdx]);
            uint64_t hi = (lsbs[wIdx] & other.msbs[wIdx])
                          ^ (msbs[wIdx] & other.lsbs[wIdx]);
            lsbs[wIdx] = lo;
            msbs[wIdx] = hi;
        }
    }
    PackedZ3& operator*=(const PackedZ3& other) {
        multiplyBy(other); return *this; 
    }

    // For each position i, returns this->at(i) if the i'th
    // bit in select is 0, or other.at(i) if the i'th bit is 1
    void mux(const PackedZ3& other, const std::vector<uint64_t>& select) {
        if (select.size() != nWords) {
            throw std::logic_error("select argument to mux has size "
                +std::to_string(select.size())+" but packed size="
                +std::to_string(SIZE));
        }
        for (int i=0; i<nWords; i++) {
            lsbs[i] = (other.lsbs[i] & select[i])
                       | (lsbs[i] & (~(select[i])));
            msbs[i] = (other.msbs[i] & select[i])
                       | (msbs[i] & (~(select[i])));
        }
    }

    // Matrix-vector multiplication
    using PackedMatrixZ3 = std::vector< PackedZ3 >;
    template<unsigned int COLS>
    void matByVec(const PackedMatrixZ3& mat, PackedZ3<COLS> &vec) {
        if (mat.size() != COLS) {
            throw std::logic_error("Multiplying a "+std::to_string(SIZE)
                +"-by-"+std::to_string(mat.size())
                +" matrix by a packed "+std::to_string(COLS)
                +" vector");
        }
        reset();      // initialized to zero
        for (int cIdx=0; cIdx<mat.size(); cIdx++) {
            int num = vec.at(cIdx);   // 0,1, or 2            
            uint64_t lsb = num & 1;
            lsb = -lsb;                  // all-0 or all-1
            uint64_t msb = num >> 1;
            msb = -msb;                  // all-0 or all-1

            // multiply the column by (msb,lsb)
            PackedZ3 col = mat[cIdx]; // a local copy
            for (int wIdx=0; wIdx<nWords; wIdx++) {
                uint64_t tmp = (col.lsbs[wIdx] & lsb)
                               ^ (col.msbs[wIdx] & msb);
                col.msbs[wIdx] = (col.lsbs[wIdx] & msb)
                                 ^ (col.msbs[wIdx] & lsb);
                col.lsbs[wIdx] = tmp;
            }
            *this += col;
        }
    }

    friend std::ostream& operator<<(std::ostream& s, const PackedZ3& v) {
        std::vector<unsigned int> a;
        v.toArray(a);
        return s<<a;
    }
    friend std::istream& operator<<(std::istream& s, PackedZ3& v) {
        std::vector<unsigned int> a;
        s>>a;
        v.fromArray(a);
        return s;
    }
};

#endif // ifndef DARKMATTER_PACKED3_H
