//
// Created by Vivek Sharma on 11/30/20.
//
#include <iostream>
#include "newprotocol.hpp"
#include <cassert>
#include "packedMod2.hpp"
#include "packedMod3.hpp"
#include "Toeplitz-by-x.hpp"
#include "OT.hpp"
#include "mains.hpp"
#include <typeinfo>//to determine the type of variables
#include "Timing.hpp"
#include <chrono>
#include "packed_PRF_central.h"
//#include "newprotocol_test.hpp"
#include "PRF.hpp"      //keep it for testing the output with PRF_centralized
#include "lookup_functions.h"
#include "OPRF.h"

//=========variables for preprocessing===========
std::vector<uint64_t> rKglobal(toeplitzWords);
PackedZ2<N_COLS> rq_global, rxglobal, v_global;
PackedZ2<N_COLS> rw1global, rw2global, rwglobal; //rw1 is rw for server and rw2 is rw for client, rw = rw1 + rw2

PackedZ3<N_COLS> p_server, p_client, p;    //p_server is p0 and p_client is p1 in documentation

//========variables for round 1============
PackedZ2<N_COLS> mx_global, mq_global;
std::vector<uint64_t> mK_global(toeplitzWords);
PackedZ2<N_COLS> ws_mask, wc_mask;



void preproc_TrustedParty()
{
    //generate rK, rq, rx, v and rw
    for (auto &w : rKglobal) w = randomWord();
    rKglobal[rKglobal.size() - 1] &= topelitzMask; // turn off extra bits at the end
    rq_global.randomize();                  //generate random values for rq
    rxglobal.randomize();                  //generate random values for rx

    v_global.toeplitzByVec(rKglobal,rxglobal);
    v_global.add(rq_global);                    //compute v = rK * rx + rq

    rw1global.randomize();
    rw2global.randomize();
    rwglobal = rw1global;
    rwglobal.add(rw2global);      //rw = rw1 + rw2

    //convert p from rw (mod2 -> mod3)
    for(int n_count = 0; n_count < N_COLS; n_count++)
    {
        p.second.set(n_count,0);
        p.first.set(n_count,rwglobal.at(n_count));
    }
    std::cout<<"Value of rw: "<<rwglobal<<std::endl;
    std::cout<<"Value of p: "<<p<<std::endl;
}

void fetch_server(std::vector<uint64_t>& rK, PackedZ2<N_COLS>& q,
                  PackedZ2<N_COLS>& rq,PackedZ2<N_COLS>& rw1) //copy global values to local variables
{
    rK = rKglobal;
    q.randomize();  //randomly generate q and send it to server
    rq = rq_global;
    rw1 = rw1global;
}

void fetch_client(PackedZ2<N_COLS>& rx,PackedZ2<N_COLS>& rw2)//copy global values to local variables
{
    rx = rxglobal;
    rw2 = rw2global;
}

//Round 1 starts here
void client_round1(PackedZ2<N_COLS>& x,PackedZ2<N_COLS>& rx)
{
    PackedZ2<N_COLS> mx;
    mx = x;
    mx.add(rx);
    mx_global = mx;     //this emulates as mx being sent from client to server
}

void server_round1(std::vector<uint64_t>& K,std::vector<uint64_t>& rK,
                   PackedZ2<N_COLS>& q, PackedZ2<N_COLS>& rq)
{
    //fetch mx first
    PackedZ2<N_COLS> mx;
    mx = mx_global;
    //mK_global = K ^ rK;
    for(int i = 0; i < toeplitzWords; i++)
    {
        mK_global[i] = K[i] ^ rK[i];
    }
    mq_global.toeplitzByVec(rK,mx);
    mq_global.add(q);
    mq_global.add(rq);
    ws_mask = q;                //w_mask for server
    ws_mask.add(rw1global);        //ws_mask = q + rw1
}

void client_round1_final(PackedZ2<N_COLS>& x,PackedZ2<N_COLS>& v,PackedZ2<N_COLS>& rw2)
{
    //fetch mK-global,mq_global,
    std::vector<uint64_t> mK;
    PackedZ2<N_COLS> mq;
    mK = mK_global;
    mq = mq_global;

    wc_mask.toeplitzByVec(mK,x);    //w_mask for client
    wc_mask.add(mq);
    wc_mask.add(v);
    wc_mask.add(rw2);   //wc_mask = (mK * x) + mq + v + rw2
}

void oblivious_PRF(std::vector<uint64_t>& K, PackedZ2<N_COLS>& x, std::vector<PackedZ3<81> >& Rmat,
                   PackedZ3<81>& y_out_z3, unsigned int nRuns)
{

    std::vector<uint64_t> rK(toeplitzWords);
    PackedZ2<N_COLS> rq, rx, v, rw, q, rw1, rw2;

    //preprocess the input
    preproc_TrustedParty();         //generates rK, rx, rq and v.

    fetch_server(rK, q, rq, rw1);
    fetch_client(rx, rw2);


    client_round1(x,rx);
    server_round1(K, rK, q, rq);
    client_round1_final(x,v,rw2);



    //server_step3
    //client_step3
}