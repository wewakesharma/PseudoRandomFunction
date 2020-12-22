#include <iostream>

#include <sodium.h>
#include <crypto_core_ed25519.h>

int main(void)
{
    if (sodium_init() == -1) {
        return 1;
    }

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

    //TODO: create a loop and time this call

    crypto_scalarmult_ed25519_base_noclamp(gr, r);
    crypto_core_ed25519_add(a, px, gr);

// -------- Second party -------- Send g^k and a^k
    unsigned char k[crypto_core_ed25519_SCALARBYTES];
    randombytes_buf(k, sizeof k);

// Compute v = g^k
    unsigned char v[crypto_core_ed25519_BYTES];
    crypto_scalarmult_ed25519_base(v, k);

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
    unsigned char fx[crypto_core_ed25519_BYTES];
    crypto_core_ed25519_add(fx, b, vir);
}
