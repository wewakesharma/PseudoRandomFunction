// Copyright (C) 2020 Tzipora Halevi, MIT license - see LICENSE file
/** Toeplitz-by-x.cpp
 *  - implementing a protocol for A*x (aka A*x+b) mod 2
 *    A is a Toeplitz matrix
 */
#include <cassert>
#include <utility>
#include <string>
#include <iostream>
#include "packedMod2.hpp"
#include "packedMod3.hpp"
#include "OT.hpp"
#include "mains.hpp"
#include <chrono>

extern long timerOTP1;
extern long timerOTP2;

extern long timerSCP1;
extern long timerSCP2;

// A place to store the results from pre-processing

std::vector< PackedZ2<N_SIZE> > rxps;
std::vector< PackedPairZ2<N_SIZE> > raps, rbps, zps;

// Methods for returning data from pre-processing
static PackedZ2<N_SIZE>& get_rx_PP(int index) {
    return rxps.at(index);
}
static PackedPairZ2<N_SIZE>& get_z_PP(int index) {
    return zps.at(index);
}
static PackedPairZ2<N_SIZE>& get_ra_PP(int index) {
    return raps.at(index);
}
static PackedPairZ2<N_SIZE>& get_rb_PP(int index) {
    return rbps.at(index);
}

// Static variables for implementing poor-man's communication channels
static PackedZ2<N_SIZE> mx_OT_global;
static PackedPairZ2<N_SIZE> m0_OT_global;
static PackedPairZ2<N_SIZE> m1_OT_global;

// Send/receive methods
static void snd_mx(const PackedZ2<N_SIZE>& mx) {
    mx_OT_global = mx;
}
static void snd_m0_m1(const PackedPairZ2<N_SIZE>&  m0, const PackedPairZ2<N_SIZE>&  m1) {
    m0_OT_global = m0;
    m1_OT_global = m1;
}
static PackedPairZ2<N_SIZE>& rcv_m0()
{
    return m0_OT_global;
}
static PackedPairZ2<N_SIZE>& rcv_m1()
{
    return m1_OT_global;
}
static PackedZ2<N_SIZE>& rcv_mx()
{
    return mx_OT_global;
}

// A "trusted party implementation" of pre-processing
// Generate randomly ra, rb pairs of binary vectors
// genrate randomly binary rx vector
// calculate z = ra*rx+ rb*(~rx)

void preProc_OT(unsigned int nTimes) {
    // allocate space
    raps.resize(nTimes);
    rbps.resize(nTimes);
    zps.resize(nTimes);
    rxps.resize(nTimes);

    // generate (ra, rb, rx, rz) tuples, as many as needed
    for (unsigned int i = 0; i < nTimes; i++) {
        randomize(raps[i]);  // random ra[i]'s
        randomize(rbps[i]);  // random rb[i]'s
        rxps[i].randomize();  // random rx[i]'s

        zps[i] = rbps[i];   // rb
        zps[i] &= rxps[i];   // rb*rx

        auto notRx = rxps[i];
        notRx.negate();
        auto temp = raps[i]; // ra
        temp &= notRx;       // ra*(~rx)

        zps[i] ^= temp;      // rb*rx + ra*(~rx)
    }
}


void OT_Party2_1(const PackedZ2<N_SIZE>& x,  int index) {
    // get rx from pre-processing
    PackedZ2<N_SIZE>& rx = get_rx_PP(index);

#ifdef DEBUG
    std::cout << "OT p2 part 1, x="<<x.at(0)<<", rx="<<rx.at(0);
#endif

    auto start = std::chrono::system_clock::now();
    // send mx = x xor rx to party1
    PackedZ2<N_SIZE> mx = x;
    mx ^= rx;
#ifdef DEBUG
    std::cout << ", sending mx="<<mx.at(0) << std::endl;
#endif

    timerOTP2 += (std::chrono::system_clock::now() - start).count();
    snd_mx(mx); // send to party1
}

/*
 * OT_Party1, receives mx from party2 and then sends back
 *    𝑚0 = mx*r0 + (~𝑚𝑥)*𝑟1 + ra
 *    𝑚1 = 𝑚𝑥*𝑟1 + (~𝑚𝑥)*𝑟0 + 𝑟𝑏
 * 
 *  mx is packed-binary and all the other quantities are pairs of packed binaries
 */
void OT_Party1(const PackedPairZ2<N_SIZE>& r0,  const PackedPairZ2<N_SIZE>& r1, int index) {
    PackedZ2<N_SIZE> mx = rcv_mx(); // receive vector mx from party2

#ifdef DEBUG
    std::cout << "OT p1, mx="<<mx.at(0) << std::endl;

    std::cout << "OT p1, r0="<<r0.first.at(0)<<r0.second.at(0) << std::endl;
    std::cout << "OT p1, r1="<<r1.first.at(0)<<r1.second.at(0) << std::endl;
#endif

    // get rA, rb from pre-processing
    const PackedPairZ2<N_SIZE>& ra = get_ra_PP(index);
    const PackedPairZ2<N_SIZE>& rb = get_rb_PP(index);

#ifdef DEBUG
    std::cout << "OT p1, ra="<<ra.first.at(0)<<ra.second.at(0) << std::endl;
    std::cout << "OT p1, rb="<<rb.first.at(0)<<rb.second.at(0) << std::endl;
#endif

    auto start = std::chrono::system_clock::now();

    PackedPairZ2<N_SIZE> m0 = rb;
    PackedPairZ2<N_SIZE> m1 = ra;

    m0 &= mx;    // mx*rb
    m1 &= mx;    // mx*ra

#ifdef DEBUG
    std::cout << "OT p1, mx*rb="<<m0.first.at(0)<<m0.second.at(0) << std::endl;
    std::cout << "OT p1, mx*ra="<<m1.first.at(0)<<m1.second.at(0) << std::endl;
#endif

    mx.negate(); // ~mx

#ifdef DEBUG
    std::cout << "OT p1, ~mx="<<mx.at(0) << std::endl;
#endif

    PackedPairZ2<N_SIZE> tmp = ra;
    tmp &= mx;   // (~mx)*ra

    m0 ^= tmp;   // ~mx*ra + mx*rb

#ifdef DEBUG
    std::cout << "OT p1, (~mx)*ra"<<tmp.first.at(0)<<tmp.second.at(0) << std::endl;
    std::cout << "OT p1, m0 = mx*r1 + (~mx)*r0 = "<<m0.first.at(0)<<m0.second.at(0) << std::endl;
#endif

    tmp = rb;
    tmp &= mx;   // (~mx)*rb
    m1 ^= tmp;   // mx*ra + (~mx)*rb

#ifdef DEBUG
    std::cout << "OT p1, tmp = (~mx)*rb = "<<tmp.first.at(0)<<tmp.second.at(0) << std::endl;
    std::cout << "OT p1, m1 = mx*ra + (~mx)*rb = "<<m1.first.at(0)<<m1.second.at(0) << std::endl;
#endif


    m0 ^= r0;    // mx*rn + (~mx)*ra + r0
    m1 ^= r1;    // mx*ra + (~mx)*rb + r1

    timerOTP1 += (std::chrono::system_clock::now() - start).count();

    snd_m0_m1(m0, m1);        // send to party2

#ifdef DEBUG
    std::cout << "OT p1, send m0=mx*rn + (~mx)*ra + r0 = "<<m0.first.at(0)<<m0.second.at(0) << std::endl;
    std::cout << "OT p1, send m1=mx*ra + (~mx)*rb + r1 = "<<m1.first.at(0)<<m1.second.at(0) << std::endl;
#endif
}

void OT_Party2_2(const PackedZ2<N_SIZE>& x, PackedPairZ2<N_SIZE>& out, int index) {

    // get rx and z frp, pre-processing
    PackedZ2<N_SIZE>& rx = get_rx_PP(index);
    PackedPairZ2<N_SIZE>& z = get_z_PP(index);

#ifdef DEBUG
    std::cout << "OT p2 part 2, z="<<z.first.at(0)<< z.second.at(0) << ", rx="<<rx.at(0) << std::endl;
#endif

    // receive m0,m1 from party1
    PackedPairZ2<N_SIZE> m0 = rcv_m0();
    out = rcv_m1(); // m1

#ifdef DEBUG
    std::cout << "OT p2 part 2, receive m0="<<m0.first.at(0)<<m0.second.at(0) << std::endl;
    std::cout << "OT p2 part 2, receive m1="<<out.first.at(0)<<out.second.at(0) << std::endl;
#endif

    auto start = std::chrono::system_clock::now();

    out &= x;      // m1*x

#ifdef DEBUG
    std::cout << "OT p2 part 2, m1*rx="<<out.first.at(0)<<out.second.at(0) << std::endl;
#endif

    m0 &= ~x;      // m0*(~X)

#ifdef DEBUG
    std::cout << "OT p2 part 2, m0*rx="<<m0.first.at(0)<<m0.second.at(0) << std::endl;
#endif

    out ^= m0;      // m0*(~X) + m1*x

#ifdef DEBUG
    std::cout << "OT p2 part 2, m0*(~rX) + m1*rx="<<out.first.at(0)<<out.second.at(0) << std::endl;
#endif

    out ^= z;       // m0*(~rX) + m1*rx - z

    timerOTP2 += (std::chrono::system_clock::now() - start).count();

#ifdef DEBUG
    std::cout << "OT p2 part 2, m0*(~rX) + m1*rx - z="<<out.first.at(0)<<out.second.at(0) << std::endl;

    std::cout << "OT p2 part 2, out= "<<out.first.at(0)<<out.second.at(0) << std::endl;
#endif

    // out is the output of this party
}

void SC_Party2_1(const PackedZ2<N_SIZE>& y2, int index) {
//    std::cout << "\nSC: party 2 calling OT("<<y2.at(0)<<",index="<<index<<")\n";
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

    OT_Party2_1(y2, index);

    timerSCP2 += (std::chrono::system_clock::now() - start).count();
}

/*
 * Get 𝑟_𝑎, 𝑟_𝑏  from preprocessing		get 𝑟_𝑥, 𝑧 from preprocessing
Choose   𝑟 ⟵Z_3
𝑟_0≔(𝑟+𝑦_1) 𝑚𝑜𝑑 3
𝑟_1≔(𝑟+1 −𝑦_1) 𝑚𝑜𝑑 3
𝑂𝑇𝑆𝑒𝑛𝑑𝑒𝑟 (𝑟_0, 𝑟_1, 𝑟_𝑎, 𝑟_𝑏)
 */
void SC_Party1(const PackedZ2<N_SIZE>& y1, PackedZ3<N_SIZE>& out, int index) {

    // prepare two PackedZ3 vectors, one with y1 and the other with ~y1 = 1-y1

    const static PackedZ2<N_SIZE> zVec; //by default constructor initializes to 0
    PackedZ3<N_SIZE> y1_3;  // initialized to zero

    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    y1_3.lsbs() = y1;       // set the lsb's to y1

#ifdef DEBUG
    assert(y1.at(0)==y1_3.at(0));
#endif

    PackedZ3<N_SIZE> noty1_3 = y1_3; // initialized to y1
    noty1_3.lsbs().negate();         // negate all the lsb's

    out.randomize(); // the output is a random vector mod 3;

#ifdef DEBUG
    std::cout << "OT p2 part 2, out= "<<out.first.at(0)<<out.second.at(0) << std::endl;
#endif

    y1_3 -= out;
    noty1_3 -= out;

    // use r0 = y1 - out mod 3 and r1 = (1-y1) - out mod 3 as the OT-sender inputs
    OT_Party1(y1_3, noty1_3, index);

    timerSCP1 += (std::chrono::system_clock::now() - start).count();

    // Note: y1_3 and noty1_3 are PackedZ3, which are derived from PackedPairZ2
    // so we can directly call OT_Party1 that expects to get PackedPairZ2's.
}


void SC_Party2_2(const PackedZ2<N_SIZE>& y2, PackedZ3<N_SIZE>& out, int index)
{
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    // Note: out is a PackedZ3, which is derived from PackedPairZ2
    // so we can directly call OT_Party2_2 that expects to get PackedPairZ2
    OT_Party2_2(y2,out, index);
//    std::cout << "\nSC: party 2 OT returns "<<out.at(0)<<std::endl;

    timerSCP2 += (std::chrono::system_clock::now() - start).count();
}


