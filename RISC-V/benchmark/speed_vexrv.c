#include "api.h"
#include "randombytes.h"
#include "hal.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "params.h"
#include "poly.h"
#include "implvariant.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define printf hal_send_str

static void printcycles(const char *s, unsigned long long c)
{
    char outs[32];
    hal_send_str(s);
    snprintf(outs, sizeof(outs), "%llu ", c);
    hal_send_str(outs);
}
// extern void ntt_2(int16_t *p, int16_t *zetas, int32_t KQ, int32_t qinv, int16_t *new_zeta_31, const int32_t barret_constant);
// extern void invntt_2(int16_t *p, int16_t *zetas, int32_t KQ, int32_t qinv, const int32_t barret_constant, int16_t *new_omega_r2);

// int16_t new_zeta_31[112] = {573, 1223, 652, 2226, 430, 555, 843, 2004, 2777, 1015, 2078, 871, 1550, 105, 264, 2036, 1491, 422, 587, 177, 3094, 383, 3047, 1785, 3038, 2869, 1574, 1653, 2500, 516, 3321, 3083, 778, 1159, 3182, 1458, 3009, 2663, 2552, 1483, 2727, 1119, 1727, 1711, 2167, 1739, 644, 2457, 349, 3199, 126, 1469, 418, 329, 3173, 3254, 2648, 2476, 3239, 817, 1097, 603, 610, 1017, 3058, 830, 1322, 2044, 1864, 384, 732, 107, 1908, 2114, 3193, 1218, 1994, 608, 3082, 2378, 2455, 220, 2142, 1670, 1787, 2931, 961, 2144, 1799, 2051, 794, 411, 1821, 2604, 1819, 2475, 2459, 478, 3124, 448, 2264, 3221, 3021, 996, 991, 1758, 677, 2054, 958, 1869, 1522, 1628};
// int16_t new_omega_r2[112] = {1701, 1807, 1460, 2371, 1275, 2652, 1571, 2338, 2333, 308, 108, 1065, 2881, 205, 2851, 870, 854, 1510, 725, 1508, 2918, 2535, 1278, 1530, 1185, 2368, 398, 1542, 1659, 1187, 3109, 874, 951, 247, 2721, 1335, 2111, 136, 1215, 1421, 3222, 2597, 2945, 1465, 1285, 2007, 2499, 271, 2312, 2719, 2726, 2232, 2512, 90, 853, 681, 75, 156, 3000, 2911, 1860, 3203, 130, 2980, 872, 2685, 1590, 1162, 1618, 1602, 2210, 602, 1846, 777, 666, 320, 1871, 147, 2170, 2551, 246, 8, 2813, 829, 1676, 1755, 460, 291, 1544, 282, 2946, 235, 3152, 2742, 2907, 1838, 1293, 3065, 3224, 1779, 2458, 1251, 2314, 552, 1325, 2486, 2774, 2899, 1103, 2677, 2106, 2756};
// // 4 layer merging
// int16_t zetas_asm_mont[128] = {
//     2571, 2970, 1812, 1493, 1422, 287, 202, 3158, 622, 1577, 182, 962, 2127, 1855, 1468,
//     // 1st loop of 3 & 2 & 1 layers
//     573, 1223, 652, 2226, 430, 555, 843, 2004, 2777, 1015, 2078, 871, 1550, 105,
//     // 2nd loop of 3 & 2 & 1 layers
//     264, 2036, 1491, 422, 587, 177, 3094, 383, 3047, 1785, 3038, 2869, 1574, 1653,
//     // 3rd loop of 3 & 2 & 1 layers
//     2500, 516, 3321, 3083, 778, 1159, 3182, 1458, 3009, 2663, 2552, 1483, 2727, 1119,
//     // 4th loop of 3 & 2 & 1 layers
//     1727, 1711, 2167, 1739, 644, 2457, 349, 3199, 126, 1469, 418, 329, 3173, 3254,
//     // 5th loop of 3 & 2 & 1 layers
//     2648, 2476, 3239, 817, 1097, 603, 610, 1017, 3058, 830, 1322, 2044, 1864, 384,
//     // 6th loop of 3 & 2 & 1 layers
//     732, 107, 1908, 2114, 3193, 1218, 1994, 608, 3082, 2378, 2455, 220, 2142, 1670,
//     // 7th loop of 3 & 2 & 1 layers
//     1787, 2931, 961, 2144, 1799, 2051, 794, 411, 1821, 2604, 1819, 2475, 2459, 478,
//     // 8th loop of 3 & 2 & 1 layers
//     3124, 448, 2264, 3221, 3021, 996, 991, 1758, 677, 2054, 958, 1869, 1522, 1628, 0};
// int16_t zetas_inv_mont[128] = {
//     1701, 1807, 1460, 2371, 2338, 2333, 308, 108, 2851, 870, 854, 1510, 2535, 1278, 1530, 1185,
//     1659, 1187, 3109, 874, 1335, 2111, 136, 1215, 2945, 1465, 1285, 2007, 2719, 2726, 2232, 2512,
//     75, 156, 3000, 2911, 2980, 872, 2685, 1590, 2210, 602, 1846, 777, 147, 2170, 2551, 246,
//     1676, 1755, 460, 291, 235, 3152, 2742, 2907, 3224, 1779, 2458, 1251, 2486, 2774, 2899, 1103,
//     1275, 2652, 1065, 2881, 725, 1508, 2368, 398, 951, 247, 1421, 3222, 2499, 271, 90, 853,
//     1860, 3203, 1162, 1618, 666, 320, 8, 2813, 1544, 282, 1838, 1293, 2314, 552, 2677, 2106,
//     1571, 205, 2918, 1542, 2721, 2597, 2312, 681, 130, 1602, 1871, 829, 2946, 3065, 1325, 2756,
//     1861, 1474, 1202, 2367, 3147, 1752, 2707, 171, 3127, 3042, 1907, 1836, 1517, 359, 758, 1441};

int speedKEM()
{
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
        t0 = hal_get_time();
        crypto_kem_keypair(pk, sk);
        t1 = hal_get_time();
        sum += (t1 - t0);
    }
    printcycles("keypair cycles: ", (sum / CRYPTO_ITERATIONS));
    hal_send_str("\n");

    sum = 0;
    for (crypto_i = 0; crypto_i < CRYPTO_ITERATIONS; crypto_i++)
    {
        // Encapsulation
        t0 = hal_get_time();
        crypto_kem_enc(ct, key_a, pk);
        t1 = hal_get_time();
        sum += (t1 - t0);
    }
    printcycles("encaps cycles: ", (sum / CRYPTO_ITERATIONS));
    hal_send_str("\n");

    sum = 0;
    for (crypto_i = 0; crypto_i < CRYPTO_ITERATIONS; crypto_i++)
    {
        // Decapsulation
        t0 = hal_get_time();
        crypto_kem_dec(key_b, ct, sk);
        t1 = hal_get_time();
        sum += (t1 - t0);
    }
    printcycles("decaps cycles: ", (sum / CRYPTO_ITERATIONS));
    hal_send_str("\n");
}

void speedNTT()
{
    poly a, b;
    int N = 1000;
    unsigned long long t0, t1, sum;

    for (int i = 0; i < 256; i++)
    {
        a.coeffs[i] = i;
    }

    sum = 0;
    for (int i = 0; i < N; i++)
    {
        t0 = hal_get_time();
        poly_ntt(&a);
        t1 = hal_get_time();
        sum += (t1 - t0);
    }
    printcycles("ntt cycles: ", (sum / N));
    hal_send_str("\n");

#ifdef optstack
    sum = 0;
    for (int i = 0; i < N; i++)
    {
        t0 = hal_get_time();
        poly_basemul(&b, &a, &a);
        t1 = hal_get_time();
        sum += (t1 - t0);
    }
    printcycles("basemul cycles: ", (sum / N));
    hal_send_str("\n");
#elif defined(optspeed)
    int32_t r_tmp[KYBER_N];
    poly_half a_prime;
    sum = 0;
    for (int i = 0; i < N; i++)
    {
        t0 = hal_get_time();
        poly_basemul_opt_16_32(r_tmp, &a, &b, &a_prime);
        t1 = hal_get_time();
        sum += (t1 - t0);
    }
    printcycles("poly_basemul_opt_16_32 cycles: ", (sum / N));
    hal_send_str("\n");

    sum = 0;
    for (int i = 0; i < N; i++)
    {
        t0 = hal_get_time();
        poly_basemul_acc_opt_32_32(r_tmp, &a, &b, &a_prime);
        t1 = hal_get_time();
        sum += (t1 - t0);
    }
    printcycles("poly_basemul_acc_opt_32_32 cycles: ", (sum / N));
    hal_send_str("\n");

    sum = 0;
    for (int i = 0; i < N; i++)
    {
        t0 = hal_get_time();
        poly_basemul_acc_opt_32_16(&a, &a, &b, &a_prime, r_tmp);
        t1 = hal_get_time();
        sum += (t1 - t0);
    }
    printcycles("poly_basemul_acc_opt_32_16 cycles: ", (sum / N));
    hal_send_str("\n");
#endif

    sum = 0;
    for (int i = 0; i < N; i++)
    {
        t0 = hal_get_time();
        poly_invntt(&a);
        t1 = hal_get_time();
        sum += (t1 - t0);
    }
    printcycles("invntt cycles: ", (sum / N));
    hal_send_str("\n");

    // const int32_t barrett_const = (1U << 26) / 3329 + 1;
    // printf("Interleave ntt cycles:\n");
    // sum = 0;
    // for (int i = 0; i < N; i++)
    // {
    //     t0 = hal_get_time();
    //     ntt_2(a.coeffs, zetas_asm_mont, 3329 << 16, 62209 << 16, new_zeta_31, barrett_const);
    //     t1 = hal_get_time();
    //     sum += (t1 - t0);
    // }
    // printf("%s\n", (sum / N));

    // printf("Interleave invntt cycles:\n");
    // sum = 0;
    // for (int i = 0; i < N; i++)
    // {
    //     t0 = hal_get_time();
    //     invntt_2(a.coeffs, zetas_inv_mont, 3329 << 16, 62209 << 16, barrett_const, new_omega_r2);
    //     t1 = hal_get_time();
    //     sum += (t1 - t0);
    // }
    // printf("%s\n", (sum / N));
}
int main(void)
{

    for (int i = 0; i < 2; i++)
    {
        printf("==========================\n");
    }
    speedKEM();
    speedNTT();

    printf("#\n");

    while (1)
        ;

    return 0;
}
