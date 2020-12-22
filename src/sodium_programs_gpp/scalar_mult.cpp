
#include <sodium.h>
#include <crypto_core_ed25519.h>
#include <iostream>
//#include "Timing.hpp"
#include <chrono>
#include <cstdint>


long timer_scalar_mult = 0;


using namespace std;

int main()
{
	unsigned char fx[crypto_core_ed25519_BYTES];
	unsigned char v[crypto_core_ed25519_BYTES];

	//===============printing unit of time===================
	using Clock = std::chrono::system_clock;
    using Duration = Clock::duration;
    //std::cout << Duration::period::num << " , " << Duration::period::den << '\n';
    float time_unit_multiplier = 1;
    if(Duration::period::den == 1000000000)
        time_unit_multiplier = 0.001; //make nanosecond to microsecond
    else if(Duration::period::den == 1000000)
        time_unit_multiplier = 1;
	//======================================================
	
	std::chrono::time_point<std::chrono::system_clock> start_timer;
	start_timer = std::chrono::system_clock::now();

	if (sodium_init() == -1) {
        return 1;
    }
	
	// -------- First party -------- Send blinded p(x)
	unsigned char x[crypto_core_ed25519_UNIFORMBYTES];
	randombytes_buf(x, sizeof x);

	// Compute px = p(x), an EC point representative for x
	unsigned char px[crypto_core_ed25519_BYTES];
	crypto_core_ed25519_from_uniform(px, x);

	// Compute a = p(x) * g^r
	unsigned char r[crypto_core_ed25519_SCALARBYTES];
	unsigned char gr[crypto_core_ed25519_BYTES];
	unsigned char a[crypto_core_ed25519_BYTES];
	crypto_core_ed25519_scalar_random(r);
	crypto_scalarmult_ed25519_base_noclamp(gr, r);
	crypto_core_ed25519_add(a, px, gr);

	// -------- Second party -------- Send g^k and a^k
	unsigned char k[crypto_core_ed25519_SCALARBYTES];
	randombytes_buf(k, sizeof k);

	for(int i = 0; i< 1000; i++)	//LOOP
	{
		// Compute v = g^k
		crypto_scalarmult_ed25519_base(v, k);
	}

	// Compute b = a^k
	unsigned char b[crypto_core_ed25519_BYTES];
	if (crypto_scalarmult_ed25519(b, k, a) != 0) {
    	return -1;
	}

	// -------- First party -------- Unblind f(x)
	// Compute vir = v^(-r)
	unsigned char ir[crypto_core_ed25519_SCALARBYTES];
	unsigned char vir[crypto_core_ed25519_BYTES];
	crypto_core_ed25519_scalar_negate(ir, r);
	crypto_scalarmult_ed25519_noclamp(vir, ir, v);

	// Compute f(x) = b * v^(-r) = (p(x) * g^r)^k * (g^k)^(-r)
	//              = (p(x) * g)^k * g^(-k) = p(x)^k
	//unsigned char fx[crypto_core_ed25519_BYTES];
	crypto_core_ed25519_add(fx, b, vir);


timer_scalar_mult += (std::chrono::system_clock::now() - start_timer).count();


std::cout<<fx<<std::endl;
std::cout<<"Time to execute libsodium scalar_mult ed25519 is : "<<(timer_scalar_mult * time_unit_multiplier)<<" microseconds"<<std::endl;
std::cout<<"Number of rounds per second "<<(1000/(timer_scalar_mult*time_unit_multiplier)*1000000)<<std::endl;

	return 0;
}
