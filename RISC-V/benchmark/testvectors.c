#include "api.h"
#include "randombytes.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

// #define CRYPTO_ITERATIONS 2
#ifdef PQRISCV_PLATFORM
#include "hal.h"
#define printf hal_send_str
#endif

typedef uint32_t uint32;

static void printbytes(const unsigned char *x, unsigned long long xlen)
{
  char outs[2*xlen+1];
  unsigned long long i;
  for(i=0;i<xlen;i++)
    sprintf(outs+2*i, "%02x", x[i]);
  outs[2*xlen] = 0;
  printf(outs);
  printf("\n");
}

int main(void)
{
  unsigned char key_a[CRYPTO_BYTES], key_b[CRYPTO_BYTES];
  unsigned char pk[CRYPTO_PUBLICKEYBYTES];
  unsigned char sendb[CRYPTO_CIPHERTEXTBYTES];
  unsigned char sk_a[CRYPTO_SECRETKEYBYTES];
  int i,j;

  printf("==========================\n");

  for(i=0;i<CRYPTO_ITERATIONS;i++)
  {
    // Key-pair generation
    crypto_kem_keypair(pk, sk_a);

    printf("Public Key: \n");
    printbytes(pk,CRYPTO_PUBLICKEYBYTES);
    printf("Secret Key: \n");
    printbytes(sk_a,CRYPTO_SECRETKEYBYTES);

    // Encapsulation
    crypto_kem_enc(sendb, key_b, pk);

    printf("Ciphertext: \n");
    printbytes(sendb,CRYPTO_CIPHERTEXTBYTES);
    printf("Shared Secret B: ");
    printbytes(key_b,CRYPTO_BYTES);

    // Decapsulation
    crypto_kem_dec(key_a, sendb, sk_a);

    printf("Shared Secret A: ");
    printbytes(key_a,CRYPTO_BYTES);

    for(j=0;j<CRYPTO_BYTES;j++)
    {
      if(key_a[j] != key_b[j])
      {
        printf("ERROR\n");
        printf("#\n");
        return -1;
      }
    }
  }

  printf("#\n");
  return 0;
}
