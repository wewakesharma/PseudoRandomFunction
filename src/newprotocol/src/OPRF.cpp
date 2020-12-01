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

    //convert p from rw and p_server from rw1 (mod2 -> mod3)
    for(int n_count = 0; n_count < N_COLS; n_count++)
    {
        p.second.set(n_count,0);
        p.first.set(n_count,rwglobal.at(n_count));

        /*p_server.second.set(n_count,0);
        p_server.first.set(n_count,rw1global.at(n_count));*/
        p_server.randomize();   //randomly generate p_server

        p_client = p;
        p_client.subtract(p_server);

    }

/*
    std::cout<<"Value of rw: "<<rwglobal<<std::endl;
    std::cout<<"Value of rw1: "<<rw1global<<std::endl;
    std::cout<<"Value of rw2: "<<rw2global<<std::endl;
    std::cout<<"Value of p: "<<p<<std::endl;
    std::cout<<"Value of p_server: "<<p_server<<std::endl;
    std::cout<<"Value of p_client: "<<p_client<<std::endl;
*/
}

void fetch_server(std::vector<uint64_t>& rK, PackedZ2<N_COLS>& q,
                  PackedZ2<N_COLS>& rq,PackedZ2<N_COLS>& rw1, PackedZ3<N_COLS>& p_server_local) //copy global values to local variables
{
    rK = rKglobal;
    q.randomize();  //randomly generate q and send it to server
    rq = rq_global;
    rw1 = rw1global;
    p_server_local = p_server;
}

void fetch_client(PackedZ2<N_COLS>& rx,PackedZ2<N_COLS>& rw2, PackedZ3<N_COLS>& p_client_local)//copy global values to local variables
{
    rx = rxglobal;
    rw2 = rw2global;
    p_client_local = p_client;
}

//Round 1 starts here
void client_round1(PackedZ2<N_COLS>& x,PackedZ2<N_COLS>& rx)    //performs mx = x - rx
{
    PackedZ2<N_COLS> mx;
    mx = x;
    mx.add(rx);
    mx_global = mx;     //this emulates as mx being sent from client to server
}

void server_round1(PackedZ2<N_COLS>& ws_mask, std::vector<uint64_t>& K,std::vector<uint64_t>& rK,
                   PackedZ2<N_COLS>& q, PackedZ2<N_COLS>& rq)   //performs mK = K - rK and mq = rK*mx + q + rq, also w' = q+rw1
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

void client_round1_final(PackedZ2<N_COLS>& wc_mask, PackedZ2<N_COLS>& x,PackedZ2<N_COLS>& v,
        PackedZ2<N_COLS>& rw2)  //computes w' = (mK*x) + mq + v + rw2
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
//================END OF ROUND 1=========================

void server_round2(PackedZ3<81>& y_server, PackedZ2<N_COLS>& ws_mask, PackedZ3<N_COLS>& p_server_local, std::vector<PackedZ3<81> >& Rmat)
{
    //compute z_server
    PackedZ3<N_COLS> z_server;
    PackedZ3<N_COLS> w_pserver;
    PackedZ3<N_COLS> ws_mod3;


    //trying mux for (w' * p_server)
    w_pserver.reset();              //==> Apparently MUX works
    w_pserver.mux(p_server_local,ws_mask.bits);

    for(int n_count = 0; n_count < N_COLS; n_count++)//converting ws_mask from mod2 to mod3
    {
        ws_mod3.second.set(n_count,0);
        ws_mod3.first.set(n_count,ws_mask.at(n_count));
    }

    /*
    std::cout<<"p_server"<<p_server<<std::endl;
    std::cout<<"ws_mask"<<ws_mask<<std::endl;
    std::cout<<"w_pserver"<<w_pserver<<std::endl;*/

    z_server = p_server_local;
    z_server.add(ws_mod3);
    z_server.add(w_pserver);

    //compute y_server
    y_server.matByVec(Rmat,z_server);
}


void client_round2(PackedZ3<81>& y_client, PackedZ2<N_COLS>& wc_mask, PackedZ3<N_COLS>& p_client_local, std::vector<PackedZ3<81> >& Rmat)
{
    //compute z_server
    PackedZ3<N_COLS> z_client;
    PackedZ3<N_COLS> w_pclient;
    PackedZ3<N_COLS> wc_mod3;


    //trying mux for (w' * p_client)
    w_pclient.reset();              //==> Apparently MUX works
    w_pclient.mux(p_client_local,wc_mask.bits);

    /*
    std::cout<<"p_client"<<p_client<<std::endl;
    std::cout<<"wc_mask"<<wc_mask<<std::endl;
    std::cout<<"w_pclient"<<w_pclient<<std::endl;*/

    z_client = p_client_local;
    z_client.add(w_pclient);            //z_client = p_client + (w' * p_client)

    //compute y_server
    y_client.matByVec(Rmat,z_client);   //y = Rmat * z_client
}


/*
 * Driver function: this is the main function which calls other functions
 */
void oblivious_PRF(std::vector<uint64_t>& K, PackedZ2<N_COLS>& x, std::vector<PackedZ3<81> >& Rmat,
                   PackedZ3<81>& y_out_z3, unsigned int nRuns)
{

    std::vector<uint64_t> rK(toeplitzWords);
    PackedZ2<N_COLS> rq, rx, v, rw, q, rw1, rw2;
    PackedZ3<N_COLS> p_server_local, p_client_local;
    PackedZ2<N_COLS> ws_mask, wc_mask;
    PackedZ3<81> y_server,y_client;

    //preprocess the input
    preproc_TrustedParty();         //generates rK, rx, rq and v.

    fetch_server(rK, q, rq, rw1,p_server_local);
    fetch_client(rx, rw2,p_client_local);


    client_round1(x,rx);
    server_round1(ws_mask, K, rK, q, rq);
    client_round1_final(wc_mask, x,v,rw2);

    server_round2(y_server, ws_mask, p_server_local,Rmat);
    client_round2(y_client, wc_mask, p_client_local,Rmat);

    y_out_z3 = y_server;
    y_out_z3.add(y_client);

    //Performing first verification.
    PackedZ2<N_COLS> w;
    w = ws_mask;
    w.add(wc_mask);

    //The above must be equal to KX +rw
    PackedZ2<N_COLS> KX;
    KX.toeplitzByVec(K,x);
    KX.add(rwglobal);

    std::cout<<"Output w "<<w<<std::endl;
    std::cout<<"KX + rw "<<KX<<std::endl;
}