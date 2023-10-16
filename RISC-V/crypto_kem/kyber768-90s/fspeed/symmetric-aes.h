#ifndef SYMMETRIC_AES_H
#define SYMMETRIC_AES_H



#define AES256CTR_BLOCKBYTES 64
#ifdef PQRISCV_PLATFORM
#include "aes_vexrv.h"
#else
#include "aes.h"
#endif
#include <stddef.h>
#include <stdint.h>

typedef struct {
    aes256ctx sk_exp;
    uint8_t iv[12];
    uint32_t ctr;
} aes256xof_ctx;

void aes256ctr_prf(uint8_t *output, size_t outlen, const uint8_t *key, uint8_t nonce);
void aes256xof_absorb(aes256xof_ctx *s, const uint8_t *key, uint8_t x, uint8_t y);
void aes256xof_squeezeblocks(uint8_t *out, size_t nblocks, aes256xof_ctx *s);
void aes256xof_ctx_release(aes256xof_ctx *s);

#endif
