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


//#define N_SIZE 256
//#define N_COLS 256



inline void multPairByX(PackedPairZ2& p, const PackedZ2<N_SIZE>& x){
    p.first &= x;
    p.second &= x;
}

static PackedZ2<N_SIZE> mx_OT_global;

static PackedPairZ2 m0_OT_global;
static PackedPairZ2 m1_OT_global;


// A place to store the results from pre-processing
//static std::vector< std::vector<uint64_t> > rAs;
static std::vector< PackedZ2<N_SIZE> > rasm, rasl, rbsm, rbsl, rzsm, rzsl;
static std::vector< PackedZ2<N_SIZE> > rxs;
static std::vector< PackedZ2<N_SIZE> > tmp;

static std::vector< PackedPairZ2 > raps;
static std::vector< PackedPairZ2 > rbps;
static std::vector< PackedPairZ2 > zps;

static PackedZ2<N_SIZE>& get_rx_PP(int index) {
    return rxs.at(index);
}

static PackedPairZ2& get_z_PP(int index) {
    return zps.at(index);
}

static std::pair < PackedZ2<N_SIZE>,PackedZ2<N_SIZE> >& get_ra_PP(int index) {
    return raps.at(index);
}

static std::pair < PackedZ2<N_SIZE>,PackedZ2<N_SIZE> >& get_rb_PP(int index) {
    return rbps.at(index);
}

static void snd_mx(const PackedZ2<N_SIZE>& mx) {
    mx_OT_global = mx;
}

static void snd_m0_m1(const std::pair < PackedZ2<N_SIZE>,PackedZ2<N_SIZE> >&  m0,
                      const std::pair < PackedZ2<N_SIZE>,PackedZ2<N_SIZE> >&  m1) {
    m0_OT_global = m0;
    m1_OT_global = m1;
}

static std::pair < PackedZ2<N_SIZE>,PackedZ2<N_SIZE> >& rcv_m0()
{
    return m0_OT_global;
}

static std::pair < PackedZ2<N_SIZE>,PackedZ2<N_SIZE> >& rcv_m1()
{
    return m1_OT_global;
}

static PackedZ2<N_SIZE>& rcv_mx()
{
    return mx_OT_global;
}

// A "trusted party implementation" of pre-processing
// Generate randomly ra, rb trinery vectors
//genrate randomly binary rx vector
//calculate z = ra*rx+ rb*(1-rx)

//we will treat the trinery vectors as two vectors
/*
 * Calculate z = = 𝑟_𝑏×𝑟_𝑥+𝑟_𝑎×(~𝑟_𝑥)
 *
 * Uses PackedPairZ2 variable which is defined in this file
 */
void preProc_OT(unsigned int nTimes) {
    // allocate space

    raps.resize(nTimes);
    rbps.resize(nTimes);
    zps.resize(nTimes);
    rxs.resize(nTimes);
    tmp.resize(nTimes);

    //generate Z
    // fill with trandom data s.t. rz[i]=rA[i]*rx[i] xor rB[i] *(1-rx[i]) for all i
    //generate ra, rb trinary vectors
    for (unsigned int i = 0; i < nTimes; i++) {

        raps[i].first.randomize(); // random ra[i]'s
        raps[i].second.randomize(); // random ra[i]'s
        rbps[i].first.randomize(); // random ra[i]'s
        rbps[i].second.randomize(); // random ra[i]'s
        rxs[i].randomize(); // random rx[i]'s

        //have a temporary variable and negate it
        zps[i] = rbps[i];
        zps[i].first.multiplyBy(rxs[i]);
        zps[i].second.multiplyBy(rxs[i]);

        PackedPairZ2 temp = raps[i];
        PackedZ2<N_SIZE> notX = ~(rxs[i]);
        temp.first.multiplyBy(notX);
        temp.second.multiplyBy(notX);
        zps[i].first.add(temp.first);
        zps[i].second.add(temp.second);

    }

}

void SC_Party2_1(PackedZ2<N_SIZE>& x, int index) {
    // get rx from pre-processing

    OT_Party2_1(x,index);
}



void OT_Party2_1(PackedZ2<N_SIZE>& x,  int index) {
    // get rx from pre-processing

    preProc_OT(index);

    PackedZ2<N_SIZE>& rx = get_rx_PP(index); // local copy

    // send mx = x xor rx to party1
    PackedZ2<N_SIZE> mx = x;
    mx.add(rx);

    snd_mx(mx); // send to party1
}

/*
 * y2 = output
 */
void SC_Party2_2( PackedZ2<N_SIZE>& y2, PackedZ3<N_SIZE>& out, int index)
{
    PackedPairZ2 outp;
    OT_Party2_2(y2, outp, index);
    out.makeFromBits(outp.first.bits, outp.second.bits); //most = first, least = second
}

/*
 * rx is from preprocessing
 */

void OT_Party2_2(PackedZ2<N_SIZE>& x, PackedPairZ2& out, int index) {

    PackedZ2<N_SIZE>& rx = get_rx_PP(index); // local copy
    PackedPairZ2& z = get_z_PP(index);

    // receive back mA, mb from party1
    const PackedPairZ2 m0 = rcv_m0();
    const PackedPairZ2 m1 = rcv_m1();


    // Send mA=A xor rA and mb=Ra*mx xor b xor rb to party2
    PackedPairZ2 m1rx = m1;

    multPairByX(m1rx,rx);

    PackedPairZ2 nm0rx = m0;

    multPairByX(nm0rx,~rx);


    out.first = ~z.first;
    out.second = ~out.second;
    out^=(m1rx);
    out^=(nm0rx);

    // out is the output of this party
}

/*
 * Get 𝑟_𝑎, 𝑟_𝑏  from preprocessing		get 𝑟_𝑥, 𝑧 from preprocessing
Choose   𝑟 ⟵Z_3
𝑟_0≔(𝑟+𝑦_1) 𝑚𝑜𝑑 3
𝑟_1≔(𝑟+1 −𝑦_1) 𝑚𝑜𝑑 3
𝑂𝑇𝑆𝑒𝑛𝑑𝑒𝑟 (𝑟_0, 𝑟_1, 𝑟_𝑎, 𝑟_𝑏)
 */
void SC_Party1(PackedZ2<N_SIZE>& y1, PackedZ3<N_SIZE>& r, int index){


    r.randomize(); //r will be our output

    const static PackedZ2<N_SIZE> zVec; //by default constructor initializes to 0

    PackedZ3<N_SIZE> y3;
    y3.makeFromBits(zVec.bits,y1.bits);

    auto noty1 = ~y1;
    PackedZ3<N_SIZE> noty3;
    noty3.makeFromBits(zVec.bits,noty1.bits); //one minus y1

    PackedZ3<N_SIZE> r0 = r;
    r0.add(y3); //mod 3
    PackedZ3<N_SIZE> r1 = r;
    r1.add(noty3); // mod 3

    PackedZ2<N_SIZE> r0m, r0l, r1m, r1l;
    r0m.makeFromBits(r0.msbs);
    r0l.makeFromBits(r0.lsbs);
    r1m.makeFromBits(r1.msbs);
    r1l.makeFromBits(r1.lsbs);

    PackedPairZ2 r0p = std::make_pair(r0m, r0l);
    PackedPairZ2 r1p = std::make_pair(r1m,r1l);

    OT_Party1(r0p,r1p,index);

    r.negate(); // negate mod 3
}


/*
 * OT_Party1
 * Calculate
 * 𝑚_0≔(~𝑚_𝑥 ) 𝑟_1+𝑚_𝑥 𝑟_0+𝑟_𝑎
𝑚_1≔𝑚_𝑥 𝑟_1+(~𝑚_𝑥 ) 𝑟_0+𝑟_𝑏

 m_0, m_1 are trinery vectors

 */
void OT_Party1(PackedPairZ2& r0,  PackedPairZ2& r1, int index) {
    // get rA, rb from pre-processing

    const PackedPairZ2& ra = get_ra_PP(index);
    const PackedPairZ2& rb = get_rb_PP(index);

    assert(ra.first.size() == r0.first.size());
    assert(ra.second.size() == r0.second.size());
    assert(rb.first.size() == r1.first.size());
    assert(rb.second.size() == r1.second.size());

    PackedZ2<N_SIZE>& mx = rcv_mx(); // receive vector mx from party2

    // Send mA=A xor rA and mb=Ra*mx xor b xor rb to party2
    std::pair < PackedZ2<N_SIZE>,PackedZ2<N_SIZE> > mxr0 = r0;

        multPairByX(mxr0, mx);

    std::pair < PackedZ2<N_SIZE>,PackedZ2<N_SIZE> > nmxr1 = r1;

        multPairByX(nmxr1,(~mx));

    std::pair < PackedZ2<N_SIZE>,PackedZ2<N_SIZE> > m0;
    m0 = ra;
    m0 ^= nmxr1;
    m0 ^= mxr0;

    // Send mA=A xor rA and mb=Ra*mx xor b xor rb to party2
    std::pair < PackedZ2<N_SIZE>,PackedZ2<N_SIZE> > mxr1 = r1;

        multPairByX(mxr1,mx);

    std::pair < PackedZ2<N_SIZE>,PackedZ2<N_SIZE> > nmxr0 = r0;

        multPairByX(nmxr0,(~mx));

    std::pair < PackedZ2<N_SIZE>,PackedZ2<N_SIZE> > m1;
    m1 = rb;
    m1^=(nmxr1);
    m1^=(mxr0);

    snd_m0_m1(m0, m1);        // send to party2

    // b is the output of this party
}



/*
 * preProc_OT2
 *
 * An implementation that uses two vectors for each trinery variable
 * One can be used to represent the MSB and one for the LSB
 *
 *  * Calculates r_z = = 𝑟_𝑏×𝑟_𝑥+𝑟_𝑎×(~𝑟_𝑥)
 */
void preProc_OT2(unsigned int nTimes) {
    // allocate space

    rasm.resize(nTimes);
    rasl.resize(nTimes);
    rbsm.resize(nTimes);
    rbsl.resize(nTimes);
    rzsm.resize(nTimes);
    rzsl.resize(nTimes);
    rxs.resize(nTimes);
    tmp.resize(nTimes);

    PackedZ2<N_SIZE> temp2;
    //generate Z
    // fill with trandom data s.t. rz[i]=rA[i]*rx[i] xor rB[i] *(1-rx[i]) for all i
    //generate ra, rb trinary vectors
    for (unsigned int i = 0; i < nTimes; i++) {

        rasm[i].randomize(); // random ra[i]'s
        rasl[i].randomize(); // random ra[i]'s
        rbsm[i].randomize(); // random rb[i]'s
        rbsl[i].randomize(); // random rb[i]'s
        rxs[i].randomize(); // random rx[i]'s

        tmp[i].randomize();

        //have a temporary variable and negate it
        rzsm[i] = rbsm[i];
        rzsm[i].multiplyBy(rxs[i]);
        tmp[i] = (~rxs[i]);
        tmp[i].multiplyBy(rasm[i]);
        rzsm[i].add(tmp[i]);

        rzsl[i] = rbsl[i];
        rzsl[i].multiplyBy(rxs[i]);
        tmp[i] = ~rxs[i];
        tmp[i].multiplyBy(rasl[i]);
        rzsl[i].add(tmp[i]);
        }

}
