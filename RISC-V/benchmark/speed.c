#include "api.h"
#include "randombytes.h"
#include "cpucycles.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "params.h"
#include "poly.h"
#include "implvariant.h"

#define MAX(a,b) (((a)>(b))?(a):(b))

#define printcycles(S, U) printf("%s:\n",S);\
    printf("%s\n",U);

static char *
ullu(uint64_t val)
{
    static char buf[21] = {0};
    buf[20] = 0;
    char *out = &buf[19];
    uint64_t hval = val;
    unsigned int hbase = 10;

    do
    {
        *out = "0123456789"[hval % hbase];
        --out;
        hval /= hbase;
    } while (hval);

    return ++out;
}

int speedKEM(){
    unsigned char key_a[CRYPTO_BYTES], key_b[CRYPTO_BYTES];
    unsigned char sk[CRYPTO_SECRETKEYBYTES];
    unsigned char pk[CRYPTO_PUBLICKEYBYTES];
    unsigned char ct[CRYPTO_CIPHERTEXTBYTES];

    unsigned long long t0, t1, sum;
    int i;
    int crypto_i;

    sum = 0;
    for (crypto_i = 0; crypto_i < CRYPTO_ITERATIONS; crypto_i++)
    {
        // Key-pair generation
        t0 = cpucycles();
        crypto_kem_keypair(pk, sk);
        t1 = cpucycles();
        sum += (t1 - t0);
    }
    printcycles("keypair cycles", ullu(sum / CRYPTO_ITERATIONS));

    sum = 0;
    for (crypto_i = 0; crypto_i < CRYPTO_ITERATIONS; crypto_i++)
    {
        // Encapsulation
        t0 = cpucycles();
        crypto_kem_enc(ct, key_a, pk);
        t1 = cpucycles();
        sum += (t1 - t0);
    }
    printcycles("encaps cycles", ullu(sum / CRYPTO_ITERATIONS));

    sum = 0;
    for (crypto_i = 0; crypto_i < CRYPTO_ITERATIONS; crypto_i++)
    {
        // Decapsulation
        t0 = cpucycles();
        crypto_kem_dec(key_b, ct, sk);
        t1 = cpucycles();
        sum += (t1 - t0);
    }
    printcycles("decaps cycles", ullu(sum / CRYPTO_ITERATIONS));
}

void speedNTT()
{
    poly a,b;
    int N=1000;
    unsigned long long t0, t1, sum;

    for (int i = 0; i < 256; i++)
    {
        a.coeffs[i] = i;
    }

    sum = 0;
    for (int i = 0; i < N; i++)
    {
        t0 = cpucycles();
        poly_ntt(&a);
        t1 = cpucycles();
        sum += (t1 - t0);
    }
    printcycles("ntt cycles", ullu(sum / N));

#ifdef optstack
    sum = 0;
    for (int i = 0; i < N; i++)
    {
        t0 = cpucycles();
        poly_basemul(&b, &a, &a);
        t1 = cpucycles();
        sum += (t1 - t0);
    }
    printcycles("basemul cycles", ullu(sum / N));
#elif defined(optspeed)
    int32_t r_tmp[KYBER_N];
    poly_half a_prime;
    sum = 0;
    for (int i = 0; i < N; i++)
    {
        t0 = cpucycles();
        poly_basemul_opt_16_32(r_tmp, &a, &b, &a_prime);
        t1 = cpucycles();
        sum += (t1 - t0);
    }
    printcycles("poly_basemul_opt_16_32 cycles", ullu(sum / N));

    sum = 0;
    for (int i = 0; i < N; i++)
    {
        t0 = cpucycles();
        poly_basemul_acc_opt_32_32(r_tmp, &a, &b, &a_prime);
        t1 = cpucycles();
        sum += (t1 - t0);
    }
    printcycles("poly_basemul_acc_opt_32_32 cycles", ullu(sum / N));

    sum = 0;
    for (int i = 0; i < N; i++)
    {
        t0 = cpucycles();
        poly_basemul_acc_opt_32_16(&a, &a, &b, &a_prime, r_tmp);
        t1 = cpucycles();
        sum += (t1 - t0);
    }
    printcycles("poly_basemul_acc_opt_32_16 cycles", ullu(sum / N));
#endif

    sum = 0;
    for (int i = 0; i < N; i++)
    {
        t0 = cpucycles();
        poly_invntt(&a);
        t1 = cpucycles();
        sum += (t1 - t0);
    }
    printcycles("invntt cycles", ullu(sum / N));

    // const int32_t barrett_const = (1U << 26) / 3329 + 1;
    // printf("Interleave ntt cycles:\n");
    // sum = 0;
    // for (int i = 0; i < N; i++)
    // {
    //     t0 = cpucycles();
    //     ntt_2(a.coeffs, zetas_asm_mont, 3329 << 16, 62209 << 16, new_zeta_31, barrett_const);
    //     t1 = cpucycles();
    //     sum += (t1 - t0);
    // }
    // printf("%s\n", ullu(sum / N));

    // printf("Interleave invntt cycles:\n");
    // sum = 0;
    // for (int i = 0; i < N; i++)
    // {
    //     t0 = cpucycles();
    //     invntt_2(a.coeffs, zetas_inv_mont, 3329 << 16, 62209 << 16, barrett_const, new_omega_r2);
    //     t1 = cpucycles();
    //     sum += (t1 - t0);
    // }
    // printf("%s\n", ullu(sum / N));
}
int main(void){

    for (int i = 0; i < 2; i++)
    {
        printf("==========================\n");
    }
    speedKEM();
    speedNTT();

    printf("#\n");

    while(1);

    return 0;
}
