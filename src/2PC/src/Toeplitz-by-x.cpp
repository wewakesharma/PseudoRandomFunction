// Copyright (C) 2020 Tzipora Halevi, MIT license - see LICENSE file
/** Toeplitz-by-x.cpp
 *  - implementing a protocol for A*x (aka A*x+b) mod 2
 *    A is a Toeplitz matrix
 */
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>


#include <cassert>
#include "packedMod2.hpp"
#include "Toeplitz-by-x.hpp"


using namespace std;
// in Toeplitz-by-x.hpp
// #define N_ROWS 256
// #define N_COLS 256

uint64_t topelitzMask; // mask to remove extra bits in the rAs
const unsigned int toeplitzWords = (N_ROWS+N_COLS-1+63)/64;

// A place to store the results from pre-processing
static std::vector< std::vector<uint64_t> > rAs;
static std::vector< PackedZ2<N_ROWS> > rbs, rzs;
static std::vector< PackedZ2<N_COLS> > rxs;

// A "trusted party implementation" of pre-processing
void preProc_Toeplitz_by_x(unsigned int nTimes) {
    // allocate space
    rAs.resize(nTimes, std::vector<uint64_t>(toeplitzWords));
    rbs.resize(nTimes);
    rzs.resize(nTimes);
    rxs.resize(nTimes);

    // fill with trandom data s.t. rz[i]=rA[i]*rx[i] xor rb[i] for all i
    for (unsigned int i=0; i<nTimes; i++) {
        // fill rAs[*] with random Topellitz matrices
        for (auto& w : rAs[i]) w = randomWord();
        rAs[i][rAs[i].size()-1] &= topelitzMask;
        
        rbs[i].randomize(); // random rb[i]'s
        rxs[i].randomize(); // random rx[i]'s
        
        // rz[i] = rA[i] * rx[i] xor rb[i]
        rzs[i].toeplitzByVec(rAs[i], rxs[i]);
        rzs[i].add(rbs[i]);
    }
}

static std::vector<uint64_t>& get_rA_PP(int index) {
    return rAs.at(index);
}
static PackedZ2<N_ROWS>& get_rb_PP(int index) {
    return rbs.at(index);
}
static PackedZ2<N_COLS>& get_rx_PP(int index) {
    return rxs.at(index);
}
static PackedZ2<N_ROWS>& get_rz_PP(int index) {
    return rzs.at(index);
}


// poor-man's implementation of communication channels

// FIXME: replace with real communication, on top of not having real
//        communication this implementation is also not thread safe

static std::vector<uint64_t> mA_global;
static PackedZ2<N_ROWS> mb_global;
static PackedZ2<N_COLS> mx_global;

void initGlobals(PackedZ2<N_ROWS>& b, const std::vector<uint64_t>& A) {
    // initialize the Topelitz mask
    int extraBits = 64*toeplitzWords - (N_ROWS+N_COLS-1);
    if (extraBits>0) {
        topelitzMask = 0x8000000000000000; // 1 at the top bit
        topelitzMask >>= (extraBits-1); // 1 at pos 63-extraBits+1
        topelitzMask = -topelitzMask;   // 1 at pos 63...63-extraBits+1
        topelitzMask = ~topelitzMask;   // 1 at pos 63-extraBits...0
    }

    mA_global.resize(toeplitzWords); // allocate space
}

static void snd_mx(const PackedZ2<N_COLS>& mx) {
    mx_global = mx;
}

static PackedZ2<N_COLS>& rcv_mx()
{
    return mx_global;
}

static void snd_mA_mb(const std::vector<uint64_t>& mA,
               const PackedZ2<N_ROWS>& mb) {
    mA_global = mA;
    mb_global = mb;
}

static std::vector<uint64_t>& rcv_mA()
{
    return mA_global;
}

static PackedZ2<N_ROWS>& rcv_mb()
{
    return mb_global;
}

/**** implementation of AX+b with A being a Toeplitz matrix *******/

void topelitz_Party1(PackedZ2<N_ROWS>& b, const std::vector<uint64_t>& A,
                     int index) {                     
    // get rA, rb from pre-processing
    const std::vector<uint64_t>& rA = get_rA_PP(index);
    PackedZ2<N_ROWS>& rb = get_rb_PP(index);
    assert(rA.size() == A.size());

    b.randomize();// choose a random vector b

    PackedZ2<N_COLS>& mx = rcv_mx(); // receive vector mx from party2

    // Send mA=A xor rA and mb=Ra*mx xor b xor rb to party2
    std::vector<uint64_t> mA = A;
    for (unsigned int i=0; i<mA.size(); i++)
        mA[i] ^= rA[i];
    
    PackedZ2<N_ROWS> mb;
    mb.toeplitzByVec(rA, mx); // rA * mx
    mb.add(b);                //    xor b
    mb.add(rb);               //    xor rb
    
    snd_mA_mb(mA, mb);        // send to party2
    
    // b is the output of this party
}

void topelitz_Party2_1(PackedZ2<N_COLS>& x, int index) {
    // get rx from pre-processing
    PackedZ2<N_COLS>& rx = get_rx_PP(index); // local copy

    // send mx = x xor rx to party1
    PackedZ2<N_COLS> mx = x;
    mx.add(rx);
    
    snd_mx(mx); // send to party1
}

void topelitz_Party2_2(PackedZ2<N_ROWS>& out, PackedZ2<N_COLS>& x,
                       int index) {
    // receive back mA, mb from party1
    const std::vector<uint64_t>& mA = rcv_mA();
    PackedZ2<N_ROWS>& mb = rcv_mb();

    // get from pre-processing rx = rA*rx xor rb
    PackedZ2<N_ROWS>& rz = get_rz_PP(index);
    out.toeplitzByVec(mA, x); // mA*x
    out.add(mb);              //   xor mb
    out.add(rz);              //   xor rz

    // out is the output of this party
}

void p1_recv()//p1 acts as a server
{
    int port = 12345;
    //buffer to send and receive messages with
    char msg[1500];

    //setup a socket and connection tools
    sockaddr_in servAddr;
    bzero((char*)&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);

    //open stream oriented socket with internet address
    //also keep track of the socket descriptor
    int serverSd = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSd < 0)
    {
        cerr << "Error establishing the server socket" << endl;
        exit(0);
    }
    //bind the socket to its local address
    int bindStatus = bind(serverSd, (struct sockaddr*) &servAddr,
                          sizeof(servAddr));
    if(bindStatus < 0)
    {
        cerr << "Error binding socket to local address" << endl;
        exit(0);
    }
    cout << "Waiting for a client to connect..." << endl;
    //listen for up to 5 requests at a time
    listen(serverSd, 5);
    //receive a request from client using accept
    //we need a new address to connect with the client
    sockaddr_in newSockAddr;
    socklen_t newSockAddrSize = sizeof(newSockAddr);
    //accept, create a new socket descriptor to
    //handle the new connection with client
    int newSd = accept(serverSd, (sockaddr *)&newSockAddr, &newSockAddrSize);
    if(newSd < 0)
    {
        cerr << "Error accepting request from client!" << endl;
        exit(1);
    }
    cout << "Connected with Party 2!" << endl;
    memset(&msg, 0, sizeof(msg));//clear the buffer
    recv(newSd, (char*)&msg, sizeof(msg), 0);
    //convert msg from istream to string and then into uint64_t(PackedZ2 data member)
    std::istr oss;
    operator<<(oss,mx);
    string data = oss.str();
}

p2_recv()//p2 acts as a server
{

}
