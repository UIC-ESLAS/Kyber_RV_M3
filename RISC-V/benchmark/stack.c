#include "api.h"
#include "randombytes.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
// https://stackoverflow.com/a/1489985/1711232
#ifdef PQRISCV_PLATFORM
  #include "hal.h"
  #define printf hal_send_str
#endif

char outs[32];
volatile unsigned char *p;
unsigned int c;
uint8_t canary = 0x42;

#define PRINTCYCLES()                     \
  snprintf(outs, sizeof(outs), "%u ", c); \
  printf(outs);                           \
  printf("\n");

#define FILL_STACK()             \
  p = &a;                        \
  while (p > (&a - canary_size)) \
    *(p--) = canary;

#define CHECK_STACK()                \
  c = canary_size;                   \
  p = &a - canary_size + 1;          \
  while ((*p == canary) && (p < &a)) \
  {                                  \
    p++;                             \
    c--;                             \
  }

uint8_t sk[CRYPTO_SECRETKEYBYTES];
uint8_t ss_a[CRYPTO_BYTES], ss_b[CRYPTO_BYTES];
// -128 for avoiding affecting heap memory
// 0x1a00 for sifive, 0x14000 for vexriscv
#define MAX_SIZE (0x1a00-128)
unsigned int canary_size = MAX_SIZE;
uint8_t *pk = sk;
uint8_t *ct = sk;
static int test_keys(void)
{
  volatile unsigned char a;
  // Alice generates a public key
  FILL_STACK()
  crypto_kem_keypair(pk, sk);
  CHECK_STACK()
  if (c >= canary_size)
    return -1;
  printf("keypair stack:\n");
  PRINTCYCLES();
  
  // printf("%d\n",c);

  // Bob derives a secret key and creates a response
  FILL_STACK()
  crypto_kem_enc(ct, ss_a, pk);
  CHECK_STACK()
  if (c >= canary_size)
    return -1;
  printf("encaps stack:\n");
  PRINTCYCLES();

  // Alice uses Bobs response to get her secret key
  FILL_STACK()
  crypto_kem_dec(ss_b, ct, sk);
  CHECK_STACK()
  if (c >= canary_size)
    return -1;
  printf("decaps stack:\n");
  PRINTCYCLES();
}

int main(void)
{
  // marker for automated benchmarks
  for (int i = 0; i < 2; i++)
  {
    printf("==========================\n");
  }
  canary_size = MAX_SIZE;
  test_keys();
  printf("#\n");
  return 0;
}