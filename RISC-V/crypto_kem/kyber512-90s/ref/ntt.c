#include <stdint.h>
#include "params.h"
#include "ntt.h"
#include "reduce.h"

const int32_t barrett_const = (1U << 26)/KYBER_Q + 1;

extern void ntt_2(int16_t *p, int16_t *zetas, int32_t KQ, int32_t qinv, int16_t *new_zeta_31,const int32_t barret_constant);
extern void invntt_2(int16_t *p,int16_t *zetas, int32_t KQ, int32_t qinv,const int32_t barret_constant,int16_t *new_omega_r2);

//ROUND 2 zetas for asm fwNTT level 3-1 
int16_t new_zeta_31[112] = {573, 1223, 652, 2226, 430, 555, 843, 2004, 2777, 1015, 2078, 871, 1550, 105, 264, 2036, 1491, 422, 587, 177, 3094, 383, 3047, 1785, 3038, 2869, 1574, 1653, 2500, 516, 3321, 3083, 778, 1159, 3182, 1458, 3009, 2663, 2552, 1483, 2727, 1119, 1727, 1711, 2167, 1739, 644, 2457, 349, 3199, 126, 1469, 418, 329, 3173, 3254, 2648, 2476, 3239, 817, 1097, 603, 610, 1017, 3058, 830, 1322, 2044, 1864, 384, 732, 107, 1908, 2114, 3193, 1218, 1994, 608, 3082, 2378, 2455, 220, 2142, 1670, 1787, 2931, 961, 2144, 1799, 2051, 794, 411, 1821, 2604, 1819, 2475, 2459, 478, 3124, 448, 2264, 3221, 3021, 996, 991, 1758, 677, 2054, 958, 1869, 1522, 1628};
//ROUND 2 omegas for asm invNTT level 1-3 
int16_t new_omega_r2[112] = {1701, 1807, 1460, 2371, 1275, 2652, 1571, 2338, 2333, 308, 108, 1065, 2881, 205, 2851, 870, 854, 1510, 725, 1508, 2918, 2535, 1278, 1530, 1185, 2368, 398, 1542, 1659, 1187, 3109, 874, 951, 247, 2721, 1335, 2111, 136, 1215, 1421, 3222, 2597, 2945, 1465, 1285, 2007, 2499, 271, 2312, 2719, 2726, 2232, 2512, 90, 853, 681, 75, 156, 3000, 2911, 1860, 3203, 130, 2980, 872, 2685, 1590, 1162, 1618, 1602, 2210, 602, 1846, 777, 666, 320, 1871, 147, 2170, 2551, 246, 8, 2813, 829, 1676, 1755, 460, 291, 1544, 282, 2946, 235, 3152, 2742, 2907, 1838, 1293, 3065, 3224, 1779, 2458, 1251, 2314, 552, 1325, 2486, 2774, 2899, 1103, 2677, 2106, 2756};

/* Code to generate zetas and zetas_inv used in the number-theoretic transform:

#define KYBER_ROOT_OF_UNITY 17

static const uint16_t tree[128] = {
  0, 64, 32, 96, 16, 80, 48, 112, 8, 72, 40, 104, 24, 88, 56, 120, 
  4, 68, 36, 100, 20, 84, 52, 116, 12, 76, 44, 108, 28, 92, 60, 124, 
  2, 66, 34, 98, 18, 82, 50, 114, 10, 74, 42, 106, 26, 90, 58, 122, 
  6, 70, 38, 102, 22, 86, 54, 118, 14, 78, 46, 110, 30, 94, 62, 126, 
  1, 65, 33, 97, 17, 81, 49, 113, 9, 73, 41, 105, 25, 89, 57, 121, 
  5, 69, 37, 101, 21, 85, 53, 117, 13, 77, 45, 109, 29, 93, 61, 125, 
  3, 67, 35, 99, 19, 83, 51, 115, 11, 75, 43, 107, 27, 91, 59, 123, 
  7, 71, 39, 103, 23, 87, 55, 119, 15, 79, 47, 111, 31, 95, 63, 127};


static int16_t fqmul(int16_t a, int16_t b) {
  return montgomery_reduce((int32_t)a*b);
}

void init_ntt() {
  unsigned int i, j, k;
  int16_t tmp[128];

  tmp[0] = MONT;
  for(i = 1; i < 128; ++i)
    tmp[i] = fqmul(tmp[i-1], KYBER_ROOT_OF_UNITY*MONT % KYBER_Q);

  for(i = 0; i < 128; ++i)
    zetas[i] = tmp[tree[i]];

  k = 0;
  for(i = 64; i >= 1; i >>= 1)
    for(j = i; j < 2*i; ++j)
      zetas_inv[k++] = -tmp[128 - tree[j]];

  zetas_inv[127] = MONT * (MONT * (KYBER_Q - 1) * ((KYBER_Q - 1)/128) % KYBER_Q) % KYBER_Q;
}

*/ 
int16_t zetas[128] = {
  2285, 2571, 2970, 1812, 1493, 1422, 287, 202, 3158, 622, 1577, 182, 962, 2127, 1855, 1468, 
  573, 2004, 264, 383, 2500, 1458, 1727, 3199, 2648, 1017, 732, 608, 1787, 411, 3124, 1758, 
  1223, 652, 2777, 1015, 2036, 1491, 3047, 1785, 516, 3321, 3009, 2663, 1711, 2167, 126, 1469, 
  2476, 3239, 3058, 830, 107, 1908, 3082, 2378, 2931, 961, 1821, 2604, 448, 2264, 677, 2054, 
  2226, 430, 555, 843, 2078, 871, 1550, 105, 422, 587, 177, 3094, 3038, 2869, 1574, 1653, 
  3083, 778, 1159, 3182, 2552, 1483, 2727, 1119, 1739, 644, 2457, 349, 418, 329, 3173, 3254, 
  817, 1097, 603, 610, 1322, 2044, 1864, 384, 2114, 3193, 1218, 1994, 2455, 220, 2142, 1670, 
  2144, 1799, 2051, 794, 1819, 2475, 2459, 478, 3221, 3021, 996, 991, 958, 1869, 1522, 1628};

int16_t zetas_inv[128] = {
  1701, 1807, 1460, 2371, 2338, 2333, 308, 108, 2851, 870, 854, 1510, 2535, 1278, 1530, 1185, 
  1659, 1187, 3109, 874, 1335, 2111, 136, 1215, 2945, 1465, 1285, 2007, 2719, 2726, 2232, 2512, 
  75, 156, 3000, 2911, 2980, 872, 2685, 1590, 2210, 602, 1846, 777, 147, 2170, 2551, 246, 
  1676, 1755, 460, 291, 235, 3152, 2742, 2907, 3224, 1779, 2458, 1251, 2486, 2774, 2899, 1103, 
  1275, 2652, 1065, 2881, 725, 1508, 2368, 398, 951, 247, 1421, 3222, 2499, 271, 90, 853, 
  1860, 3203, 1162, 1618, 666, 320, 8, 2813, 1544, 282, 1838, 1293, 2314, 552, 2677, 2106, 
  1571, 205, 2918, 1542, 2721, 2597, 2312, 681, 130, 1602, 1871, 829, 2946, 3065, 1325, 2756, 
  1861, 1474, 1202, 2367, 3147, 1752, 2707, 171, 3127, 3042, 1907, 1836, 1517, 359, 758, 1441};


/*************************************************
* Name:        fqmul
*
* Description: Multiplication followed by Montgomery reduction
*
* Arguments:   - int16_t a: first factor
*              - int16_t b: second factor
*
* Returns 16-bit integer congruent to a*b*R^{-1} mod q
**************************************************/
static int16_t fqmul(int16_t a, int16_t b) {
  return montgomery_reduce((int32_t)a*b); 
}

/*************************************************
* Name:        ntt
*
* Description: Inplace number-theoretic transform (NTT) in Rq
*              input is in standard order, output is in bitreversed order
*
* Arguments:   - int16_t r[256]: pointer to input/output vector of elements of Zq
**************************************************/
void ntt(int16_t r[256]) {

 ntt_2(r,zetas, KYBER_Q<<16, QINV<<16,new_zeta_31,barrett_const);
 
/* unsigned int len, start, j, k;
  int16_t t, zeta;

  k = 1;
  for(len = 128; len >= 2; len >>= 1) {
    for(start = 0; start < 256; start = j + len) {
      zeta = zetas[k++];
      for(j = start; j < start + len; ++j) {
        t = fqmul(zeta, r[j + len]);
        r[j + len] = r[j] - t;
        r[j] = r[j] + t;
      }
    }
  }*/
}

/*************************************************
* Name:        invntt
*
* Description: Inplace inverse number-theoretic transform in Rq
*              input is in bitreversed order, output is in standard order
*
* Arguments:   - int16_t r[256]: pointer to input/output vector of elements of Zq
**************************************************/
void invntt(int16_t r[256]) {
   invntt_2(r, zetas_inv, KYBER_Q, QINV<<16,barrett_const, new_omega_r2);

  /*unsigned int start, len, j, k;
  int16_t t, zeta;

  k = 0;
  for(len = 2; len <= 128; len <<= 1) {
    for(start = 0; start < 256; start = j + len) {
      zeta = zetas_inv[k++];
      for(j = start; j < start + len; ++j) {
        t = r[j];
        r[j] = barrett_reduce(t + r[j + len]);
        r[j + len] = t - r[j + len];
        r[j + len] = fqmul(zeta, r[j + len]);
      }
    }
  }

  for(j = 0; j < 256; ++j)
    r[j] = fqmul(r[j], zetas_inv[127]);*/
}

/*************************************************
* Name:        basemul
*
* Description: Multiplication of polynomials in Zq[X]/((X^2-zeta))
*              used for multiplication of elements in Rq in NTT domain
*
* Arguments:   - int16_t r[2]: pointer to the output polynomial
*              - const int16_t a[2]: pointer to the first factor
*              - const int16_t b[2]: pointer to the second factor
*              - int16_t zeta: integer defining the reduction polynomial
**************************************************/
void basemul(int16_t r[2], const int16_t a[2], const int16_t b[2], int16_t zeta) {
  r[0]  = fqmul(a[1], b[1]);
  r[0]  = fqmul(r[0], zeta);
  r[0] += fqmul(a[0], b[0]);

  r[1]  = fqmul(a[0], b[1]);
  r[1] += fqmul(a[1], b[0]);
}
