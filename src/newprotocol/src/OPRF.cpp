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

//=========variables for timings=========
long timer_client_r1 = 0;
long timer_server_r1 = 0;
long timer_client_final = 0;

long timer_client_r2 = 0;
long timer_server_r2 = 0;

long timer_w_mask = 0;

long timer_client_r3 = 0;
long timer_server_r3 = 0;

long timer_round1_oprf = 0;
long timer_round2_oprf = 0;
long timer_round3_oprf = 0;

long timer_oprf = 0;

//=========variables for preprocessing===========
std::vector<uint64_t> rKglobal(toeplitzWords);
PackedZ2<N_COLS> rq_global, rxglobal, v_global;
PackedZ2<N_COLS> rw1global, rw2global, rwglobal; //rw1 is rw for server and rw2 is rw for client, rw = rw1 + rw2

PackedZ3<N_COLS> p_server, p_client, p;    //p_server is p0 and p_client is p1 in documentation. p = p_server + p_client(mod3)

//========variables for round 1============

static PackedZ2<N_COLS> x_mask_global;
static std::vector<uint64_t>K_mask_global(toeplitzWords);

//=========variable for round 2=====
PackedZ2<N_COLS> ws_global, wc_global;  //server and client share of w'
PackedZ2<N_COLS> w_global;              //w' = ws' + wc'(accesssible to both client and server)



void preproc_TrustedParty()     //preprocessing part where rK, rx, rq, v, and rw are generated
{
    //generate rK, rq, rx, v and rw
    for (auto &w : rKglobal) w = randomWord();
    rKglobal[rKglobal.size() - 1] &= topelitzMask; // turn off extra bits at the end
    rq_global.randomize();                  //generate random values for rq
    rxglobal.randomize();                  //generate random values for rx

#ifdef DEBUG
    rxglobal.reset();
    rq_global.reset();
    rKglobal={0,0,0,0,0,0,0,0};
#endif
    v_global.toeplitzByVec(rKglobal,rxglobal);
    v_global.add(rq_global);                    //compute v = rK * rx + rq

    rw1global.randomize();
    rw2global.randomize();
#ifdef DEBUG
    rw1global.reset();
    rw2global.reset();
    //rw1global.set(0,1);
    rw2global.set(0,1);
#endif

    rwglobal = rw1global;
    rwglobal.add(rw2global);      //rw = rw1 + rw2

    //convert p from rw and p_server from rw1 (mod2 -> mod3)
    for(int n_count = 0; n_count < N_COLS; n_count++)
    {
        p.second.set(n_count,0);                    //setting msb bit as 0
        p.first.set(n_count,rwglobal.at(n_count));      //set lsb bit equal to rw_global(so, p = rw)

        p_server.randomize();   //randomly generate p_server

#ifdef DEBUG
    p_server.reset();
#endif
        p_client = p;
        p_client.subtract(p_server);

    }

#ifdef OPRF_PRINT_VAL
    std::cout<<"OPRF.cpp/preproc_TrustedParty()"<<std::endl;
    std::cout<<"Value of rw1: "<<rw1global<<std::endl;
    std::cout<<"Value of rw2: "<<rw2global<<std::endl;
    std::cout<<"Value of rw: "<<rwglobal<<std::endl;
    std::cout<<"Value of p: "<<p<<std::endl;
    std::cout<<" p_server: "<<p_server<<std::endl;
    std::cout<<" p_client: "<<p_client<<std::endl<<std::endl;
#endif
}

void fetch_server(std::vector<uint64_t>& rK, PackedZ2<N_COLS>& q,
                  PackedZ2<N_COLS>& rq,PackedZ2<N_COLS>& rw1, PackedZ3<N_COLS>& p_server_local) //copy global values to local variables
{
    rK = rKglobal;
    q.randomize();  //randomly generate q and send it to server

#ifdef DEBUG
    q.reset();
#endif

    rq = rq_global;
    rw1 = rw1global;
    p_server_local = p_server;
}

void fetch_client(PackedZ2<N_COLS>& rx,PackedZ2<N_COLS>& rw2, PackedZ2<N_COLS>& v, PackedZ3<N_COLS>& p_client_local)//copy global values to local variables
{
    rx = rxglobal;
    rw2 = rw2global;
    v = v_global;
    p_client_local = p_client;
}

void send_x_mask(PackedZ2<N_COLS>& x_mask)
{
    x_mask_global = x_mask;
}

void recv_x_mask(PackedZ2<N_COLS>& x_mask)
{
    x_mask = x_mask_global;
}

void send_K_mask(const std::vector<uint64_t>& K_mask)   //save computed wc_mask in a global variable
{
    K_mask_global = K_mask;
}

void recv_K_mask(std::vector<uint64_t>& K_mask)
{
    K_mask = K_mask_global;
}
void send_ws(PackedZ2<N_COLS>& ws_mask)     //save computed wc_mask in a global variable
{
    ws_global = ws_mask;
}
void recv_ws(PackedZ2<N_COLS>& ws_mask)
{
     ws_mask = ws_global;
}
void send_wc(PackedZ2<N_COLS>& wc_mask) //save computed wc_mask in a global variable
{
    wc_global = wc_mask;
}

void recv_wc(PackedZ2<N_COLS>& wc_mask)
{
    wc_mask = wc_global;
}
void recv_w_mask(PackedZ2<N_COLS>& w_mask)
{
    w_mask = w_global;
}

//Round 1 starts here
void client_round1(PackedZ2<N_COLS>& x,PackedZ2<N_COLS>& rx)    //performs mx = x - rx
{
    PackedZ2<N_COLS> x_mask;
    x_mask = x;
    x_mask.add(rx);
    send_x_mask(x_mask);         //saves the calculated value of K ^ rK by client
#ifdef OPRF_PRINT_VAL
    std::cout<<"OPRF.cpp/client_round1():"<<std::endl;
    std::cout<<"x_mask: "<<x_mask<<std::endl<<std::endl;
#endif
}

void server_round1(std::vector<uint64_t>& K, std::vector<uint64_t>& rK)   //performs mK = K - rK and mq = rK*mx + q + rq, also w' = q+rw1
{
    std::vector<uint64_t> K_mask;

    K_mask.resize(toeplitzWords);
    for(int count = 0; count < K_mask.size(); count++)//since K and rK are of same size take any one of them for bound condition
    {
        K_mask[count] = K[count] ^ rK[count];
    }
    send_K_mask(K_mask);        //saves the calculated value of K ^ rK by server

#ifdef OPRF_PRINT_VAL
    //std::cout<<"(rK*mx) "<<mq<<std::endl;
#endif

}

//================END OF ROUND 1=========================

void server_round2(std::vector<uint64_t>& K, PackedZ2<N_COLS>& rq, PackedZ2<N_COLS>& rw1)//computing ws_mask = K*x_mask - rq + rw1
{

    //step 1: declare local x_mask and receive x_mask from global variable
    PackedZ2<N_COLS> x_mask, ws_mask;
    recv_x_mask(x_mask);

    //step2: compute K*x_mask
    PackedZ2<N_COLS> K_x_mask;
    K_x_mask.toeplitzByVec(K,x_mask);

    //step3: subtract and add rq and rw1 respectively.
    ws_mask = K_x_mask;
    ws_mask.add(rq);
    ws_mask.add(rw1);  //ws_mask = K*x_mask - rq + rw1

    //step4: send/save ws_mask as global value
    send_ws(ws_mask);


    PackedZ2<N_COLS> wc_mask,w_mask;

#ifdef OPRF_PRINT_VAL
    std::cout<<"OPRF.cpp/server_round2(): Printing values for server round 2:"<<std::endl;

#endif
}


void client_round2(PackedZ2<N_COLS>& rx, PackedZ2<N_COLS>& v, PackedZ2<N_COLS>& rw2) //computing wc = (K_mask * rx) + v + rw2
{
    PackedZ2<N_COLS> wc_mask;
    std::vector<uint64_t> K_mask(toeplitzWords);
    K_mask.resize(toeplitzWords);

    //step 1: receive K_mask
    recv_K_mask(K_mask);

    //step2: compute K_mask * rx
    PackedZ2<N_COLS> K_mask_rx;
    K_mask_rx.toeplitzByVec(K_mask, rx);

    //step3: add v and rw2 to the result of step 2
    wc_mask = K_mask_rx;
    wc_mask.add(v);
    wc_mask.add(rw2);       //wc_mask = K*rx + v +rw2

    //step4: send/save wc_mask as global value
    send_wc(wc_mask);

#ifdef OPRF_PRINT_VAL
    std::cout<<"OPRF.cpp/client_round2(): Printing values for client round 2:"<<std::endl;

#endif
}

void compute_w_mask()   //fetch ws_mask and wc_mask and compute their addition
{
    w_global = ws_global;
    w_global.add(wc_global);//w = wc + ws
}

void server_round3(PackedZ3<81>& y_server, PackedZ3<N_COLS>& p_server_local, std::vector<PackedZ3<81> >& Rmat)
{
    //receive w_mask
    PackedZ2<N_COLS> w_mask;
    recv_w_mask(w_mask);

    //compute z_server
    PackedZ3<N_COLS> z_server;
    PackedZ3<N_COLS> w_pserver;
    PackedZ3<N_COLS> w_mod3;


    //trying mux for (w' * p_server)
    w_pserver.reset();
    w_pserver.mux(p_server_local,w_mask.bits);

    for(int n_count = 0; n_count < N_COLS; n_count++)//converting w_mask from mod2 to mod3
    {
        w_mod3.second.set(n_count,0);                  //setting msb as zero(0)
        w_mod3.first.set(n_count,w_mask.at(n_count));     //ws_mask converted for compatibility reasons
    }

    z_server = p_server_local;  //computing z_server = p + w' + (p * w')
    z_server.add(w_mod3);
    z_server.add(w_pserver);

    //compute y_server = Rmat * z_server
    y_server.matByVec(Rmat,z_server);

#ifdef OPRF_PRINT_VAL
    std::cout<<"OPRF.cpp/server_round3(): Printing values for server round 3:"<<std::endl;
    std::cout<<"p_server"<<p_server_local<<std::endl;
    std::cout<<"w_mask"<<w_mask<<std::endl;
    std::cout<<"w_mod3"<<w_mod3<<std::endl;
    std::cout<<"w_pserver"<<w_pserver<<std::endl;
    std::cout<<"z_server"<<z_server<<std::endl;
    std::cout<<"y_server"<<y_server<<std::endl<<std::endl;
#endif
}
void client_round3(PackedZ3<81>& y_client, PackedZ3<N_COLS>& p_client_local, std::vector<PackedZ3<81> >& Rmat)
{
    //receive w_mask
    PackedZ2<N_COLS> w_mask;
    recv_w_mask(w_mask);

       //compute z_client
       PackedZ3<N_COLS> z_client;
       PackedZ3<N_COLS> w_pclient;


       //trying mux for (w' * p_client)
       w_pclient.reset();              //==> Apparently MUX works
       w_pclient.mux(p_client_local,w_mask.bits);

       z_client = p_client_local;
       z_client.add(w_pclient);            //z_client = p_client + (w' * p_client)

       //compute y_server
       y_client.matByVec(Rmat,z_client);   //y = Rmat * z_client

#ifdef OPRF_PRINT_VAL
    std::cout<<"OPRF.cpp/client_round3(): Printing values for client round 3:"<<std::endl;
    std::cout<<"p_client"<<p_client<<std::endl;
    std::cout<<"w_mask"<<w_mask<<std::endl;
    std::cout<<"w_pclient"<<w_pclient<<std::endl;
    std::cout<<"z_client"<<z_client<<std::endl;
    std::cout<<"y_client"<<y_client<<std::endl<<std::endl;
#endif
}

void display_oprf_timings()
{
    using Clock = std::chrono::system_clock;
    using Duration = Clock::duration;
    //std::cout << Duration::period::num << " , " << Duration::period::den << '\n';
    float time_unit_multiplier = 1;
    if(Duration::period::den == 1000000000)
        time_unit_multiplier = 0.001; //make nanosecond to microsecond
    else if(Duration::period::den == 1000000)
        time_unit_multiplier = 1;   //keep the unit as microsecond

    timer_round1_oprf = timer_client_r1 + timer_server_r1 + timer_client_final;
    timer_round2_oprf = std::max(timer_server_r2, timer_client_r2) + timer_w_mask;
    timer_round3_oprf = std::max(timer_server_r3, timer_client_r3);
    timer_oprf = timer_round1_oprf + timer_round2_oprf;

    std::cout<<"Time to execute step 1: "<<(timer_round1_oprf * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"Time to execute step 2(both party runs simultaneously): "<<(timer_round2_oprf * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"============================================================"<<std::endl;
    std::cout<<"Number of rounds per second for step 1 "<<(1000/(timer_round1_oprf*time_unit_multiplier)*1000000)<<std::endl;
    std::cout<<"Number of rounds per second for step 2 "<<(1000/(timer_round2_oprf*time_unit_multiplier)*1000000)<<std::endl;
    std::cout<<"Number of rounds per second for entire PRF "<<(1000/(timer_oprf*time_unit_multiplier)*1000000)<<std::endl;
    std::cout<<"=========================Breaking down the timings by parties for each round==================================="<<std::endl;
    std::cout<<"Time to execute client step 1: "<<(timer_client_r1 * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"Time to execute server step 1: "<<(timer_server_r1 * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"Time to execute client step 1 final: "<<(timer_client_final * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"Time to execute client step 2: "<<(timer_client_r2 * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"Time to execute server step 2: "<<(timer_server_r2 * time_unit_multiplier)<<" microseconds"<<std::endl;
    std::cout<<"=========================TOTAL TIME==================================="<<std::endl;
    std::cout<<"Time to execute entire new protocol PRF: "<<(timer_oprf * time_unit_multiplier)<<" microseconds"<<std::endl;

}


/*
 * Driver function: this is the main function which calls other functions
 */
void oblivious_PRF(std::vector<uint64_t>& K, PackedZ2<N_COLS>& x, std::vector<PackedZ3<81> >& Rmat,
                   PackedZ3<81>& y_out_z3, unsigned int nRuns)
{

    std::chrono::time_point<std::chrono::system_clock> start_client_r1, start_server_r1,start_server_r3,start_client_r3;
    std::chrono::time_point<std::chrono::system_clock> start_server_r2, start_client_r2, start_w_mask, start_timer_oprf;

    std::vector<uint64_t> rK(toeplitzWords);
    PackedZ2<N_COLS> rq, rx, v, rw, q, rw1, rw2;    //local variables fetched through preprocessing
    PackedZ3<N_COLS> p_server_local, p_client_local;

    //local variables for round 1
    PackedZ2<N_COLS> x_mask;
    std::vector<uint64_t> K_mask(toeplitzWords);

    //local variable for round 2
    PackedZ2<N_COLS> ws_mask, wc_mask;

    PackedZ3<81> y_server,y_client, y_dummy;

    //preprocess the input
    preproc_TrustedParty();         //generates rK, rx, rq and v.

    fetch_server(rK, q, rq, rw1,p_server_local);
    fetch_client(rx, rw2,v, p_client_local);

    std::cout<<" Number of Runs: "<<nRuns<<std::endl;

    //start_timer_oprf = std::chrono::system_clock::now();

    for(unsigned int i = 0; i< 1;i++) {

        start_client_r1 = std::chrono::system_clock::now();
        client_round1(x, rx);        //computes x_mask = x ^ rx
        timer_client_r1 += (std::chrono::system_clock::now() - start_client_r1).count();

        start_server_r1 = std::chrono::system_clock::now();
        server_round1(K, rK);   //computes k_mask = K ^ rK
        timer_server_r1 += (std::chrono::system_clock::now() - start_server_r1).count();

        start_server_r2 = std::chrono::system_clock::now();
        server_round2(K,rq, rw1);   //computes ws' = K*x' -rq + rw1
        timer_server_r2 += (std::chrono::system_clock::now() - start_server_r2).count();

        start_client_r2 = std::chrono::system_clock::now();
        client_round2(rx, v, rw2);      //computes wc' = K'*x + v + rw2
        timer_client_r2 += (std::chrono::system_clock::now() - start_client_r2).count();

        start_w_mask = std::chrono::system_clock::now();
        compute_w_mask();
        timer_w_mask += (std::chrono::system_clock::now() - start_w_mask).count();


        start_server_r3 = std::chrono::system_clock::now();
        server_round3(y_server,p_server_local, Rmat);
        timer_server_r3 += (std::chrono::system_clock::now() - start_server_r3).count();

        start_client_r3 = std::chrono::system_clock::now();
        client_round3(y_client,p_client_local, Rmat);
        timer_client_r3 += (std::chrono::system_clock::now() - start_client_r3).count();


        y_out_z3 = y_server;
        y_out_z3.add(y_client);


        y_dummy += y_out_z3;
    }
    std::cout << "inside oblivious_PRF, y_dummy=" << y_dummy << std::endl;
    display_oprf_timings();

#ifdef OPRF_PRINT_VAL
    //Performing first verification.
    PackedZ2<N_COLS> w;
    w = ws_global;
    w.add(wc_global);

    //The above must be equal to KX +rw
    PackedZ2<N_COLS> KX;
    KX.toeplitzByVec(K,x);
    KX.add(rwglobal);
    std::cout<<"Output w "<<w<<std::endl;
    std::cout<<"KX + rw "<<KX<<std::endl;
    if(w == KX)
        std::cout<<"Round 1 verification test status : PASS"<<std::endl<<std::endl;
    else
        std::cout<<"Round 1 verification test status : FAIL"<<std::endl<<std::endl;
#endif
}