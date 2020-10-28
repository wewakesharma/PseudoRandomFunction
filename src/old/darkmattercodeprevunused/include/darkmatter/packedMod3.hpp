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
#include "packedMod2.hpp"


/**************** A pair of packed vectors mod 2 *******************/

template <size_t SIZE>
using PackedPairZ2 = std::pair< PackedZ2<SIZE>, PackedZ2<SIZE>  >;

// bitwise XOR
template <size_t SIZE> inline PackedPairZ2<SIZE>& operator^=(
        PackedPairZ2<SIZE>& fPPZ2, const PackedPairZ2<SIZE>& sPPZ2) {
    fPPZ2.first ^= sPPZ2.first;
    fPPZ2.second ^= sPPZ2.second;
    return fPPZ2;
}

// bitwise AND
template <size_t SIZE> inline PackedPairZ2<SIZE>& operator&=(
        PackedPairZ2<SIZE>& fPPZ2, const PackedPairZ2<SIZE>& sPPZ2) {
    fPPZ2.first &= sPPZ2.first;
    fPPZ2.second &= sPPZ2.second;
    return fPPZ2;
}
template <size_t SIZE>
inline void multPairByX(PackedPairZ2<SIZE>& p, const PackedZ2<SIZE>& x){
    p.first &= x;
    p.second &= x;
}
template <size_t SIZE> inline PackedPairZ2<SIZE>& operator&=(
        PackedPairZ2<SIZE>& p, const PackedZ2<SIZE>& x) {
    multPairByX(p,x);
    return p;
}

// bitwise negation
template <size_t SIZE>
inline PackedPairZ2<SIZE> negate(const PackedPairZ2<SIZE>& pair) {
    pair.first.negate();
    pair.second.negate();
}
template <size_t SIZE>
inline PackedPairZ2<SIZE> operator~(const PackedPairZ2<SIZE>& pair) {
    PackedPairZ2<SIZE> tmp = pair;
    tmp.negate();
    return tmp;
}

// Choose two random binary vectors
template <size_t SIZE> void randomize(PackedPairZ2<SIZE>& fPPZ2) {
    fPPZ2.first.randomize();
    fPPZ2.second.randomize();
}

/*******************************************************************/


/**
 * PackedZ3 - a packed vector of Z_3 elements
 *
 * represented as two vectors for the msb's and lsb's (a PackedPairZ2)
 **/
template <size_t SIZE>
class PackedZ3: public PackedPairZ2<SIZE> {
public:
    PackedZ2<SIZE>& lsbs() { return this->first; }
    PackedZ2<SIZE>& msbs() { return this->second; }
    const PackedZ2<SIZE>& lsbs() const { return this->first; }
    const PackedZ2<SIZE>& msbs() const { return this->second; }

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
            this->first.bits[nWords-1] &= mask;
            this->second.bits[nWords-1] &= mask;
        }
    }

public:
    // set to all-zero
    void reset() { this->second.bits.assign(nWords,0); this->first.bits.assign(nWords,0);}

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

        uint64_t msb = (this->second.bits[wIdx] >> idxInWord) & 1;
        uint64_t lsb = (this->first.bits[wIdx] >> idxInWord) & 1;
 
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
        this->second.bits[wIdx] = (this->second.bits[wIdx] & mask) | msb;
        this->first.bits[wIdx] = (this->first.bits[wIdx] & mask) | lsb;
    }

    bool isZero() const {
        for (int wIdx=0; wIdx<nWords; wIdx++) {
            if (this->second.bits[wIdx] != 0 || this->first.bits[wIdx] != 0)
                return false;
        }
        return true;
    }

    bool operator==(const PackedZ3& other) const {
        return (this->second.bits==other.second.bits && this->first.bits==other.first.bits);
    }
    bool operator!=(const PackedZ3& other) const {
        return (this->second.bits!=other.second.bits || this->first.bits!=other.first.bits);
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
            this->second.bits[wIdx] = this->first.bits[wIdx] = 0;
            for (int idxInWord=0; idxInWord<64; idxInWord++,i++) {
                if (i >= SIZE) break;
                uint64_t val = src[i] % 3; // val is in 0,1,2
                uint64_t lsb = (val & 1) << idxInWord;
                uint64_t msb = ((val& 2)>>1) << idxInWord;
                this->second.bits[wIdx] |= msb;
                this->first.bits[wIdx] |= lsb;
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
                uint64_t lsb = (this->first.bits[wIdx] >> idxInWord) & 1;
                uint64_t msb = (this->second.bits[wIdx] >> idxInWord) & 1;
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
            this->second.bits[wIdx] = hi[wIdx] & lessThan3;
            this->first.bits[wIdx] = lo[wIdx] & lessThan3;
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
            this->second.bits[wIdx] = hi;
            this->first.bits[wIdx] = lo;
        }
        zeroExtraBits(); // zero out unused portion of last word (if any)
    }

    // Negation: if <m,l> \in Z_3 then <l,m> = -<m,l> (mod 3)
    void negate() { this->second.bits.swap(this->first.bits); };
    PackedZ3 operator-() const { 
        PackedZ3 tmp = *this; 
        tmp.negate(); 
        return tmp;
    }

    // Addition
    void add(const PackedZ3& other) {
        for (int wIdx=0; wIdx<nWords; wIdx++) {

            uint64_t m1 = this->second.bits[wIdx];
            uint64_t l1 = this->first.bits[wIdx];
            uint64_t m2 = other.second.bits[wIdx];
            uint64_t l2 = other.first.bits[wIdx];

            uint64_t T = (l1 | m2) ^ (l2 | m1);
            this->second.bits[wIdx] = (l1 | l2 ) ^ T;
            this->first.bits[wIdx] = (m1 | m2 ) ^ T;

        }
    }
    PackedZ3& operator+=(const PackedZ3& other) {
        add(other); return *this; 
    }
    
    // Subtraction
    void subtract(const PackedZ3& other) {
        for (int wIdx=0; wIdx<nWords; wIdx++) {

            uint64_t m1 = this->second.bits[wIdx];
            uint64_t l1 = this->first.bits[wIdx];
            uint64_t m2 = other.first.bits[wIdx];
            uint64_t l2 = other.second.bits[wIdx];

            uint64_t T = (l1 | m2) ^ (l2 | m1);
            this->second.bits[wIdx] = (l1 | l2 ) ^ T;
            this->first.bits[wIdx] = (m1 | m2 ) ^ T;


/*            uint64_t m1 = this->second.bits[wIdx];
            uint64_t l1 = this->first.bits[wIdx];
            uint64_t l2 = other.second.bits[wIdx];
            uint64_t m2 = other.first.bits[wIdx];
            uint64_t z1 = (~m1)&(~l1); // this==0
            uint64_t z2 = (~m2)&(~l2); // other==0
            this->first.bits[wIdx] = (m1&m2) | (l1&z2) | (z1&l2);
            this->second.bits[wIdx] = (l1&l2) | (m1&z2) | (z1&m2);*/
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
            uint64_t lo = (this->first.bits[wIdx] & other.first.bits[wIdx])
                          ^ (this->second.bits[wIdx] & other.second.bits[wIdx]);
            uint64_t hi = (this->first.bits[wIdx] & other.second.bits[wIdx])
                          ^ (this->second.bits[wIdx] & other.first.bits[wIdx]);
            this->first.bits[wIdx] = lo;
            this->second.bits[wIdx] = hi;
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
            this->first.bits[i] = (other.first.bits[i] & select[i])
                       | (this->first.bits[i] & (~(select[i])));
            this->second.bits[i] = (other.second.bits[i] & select[i])
                       | (this->second.bits[i] & (~(select[i])));
        }
    }

    // Matrix-vector multiplication
    using PackedMatrixZ3 = std::vector< PackedZ3 >;
    template<size_t COLS>
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
                uint64_t tmp = (col.first.bits[wIdx] & lsb)
                               ^ (col.second.bits[wIdx] & msb);
                col.second.bits[wIdx] = (col.first.bits[wIdx] & msb)
                                 ^ (col.second.bits[wIdx] & lsb);
                col.first.bits[wIdx] = tmp;
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
