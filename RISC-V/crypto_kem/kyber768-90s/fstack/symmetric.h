#ifndef SYMMETRIC_H
#define SYMMETRIC_H
#include "params.h"
#include "sha2.h"
#include "symmetric-aes.h"
#include <stddef.h>
#include <stdint.h>

typedef aes256xof_ctx xof_state;

void kyber_aes256xof_absorb(aes256xof_ctx *state, const uint8_t seed[32], uint8_t x, uint8_t y);

void kyber_aes256ctr_prf(uint8_t *out, size_t outlen, const uint8_t key[32], uint8_t nonce);

#define XOF_BLOCKBYTES AES256CTR_BLOCKBYTES

#define hash_h(OUT, IN, INBYTES) sha256(OUT, IN, INBYTES)
#define hash_g(OUT, IN, INBYTES) sha512(OUT, IN, INBYTES)
#define xof_absorb(STATE, SEED, X, Y) aes256xof_absorb(STATE, SEED, X, Y)
#define xof_squeezeblocks(OUT, OUTBLOCKS, STATE) aes256xof_squeezeblocks(OUT, OUTBLOCKS, STATE)
#define xof_ctx_release(STATE) aes256xof_ctx_release(STATE)
#define prf(OUT, OUTBYTES, KEY, NONCE) aes256ctr_prf(OUT, OUTBYTES, KEY, NONCE)
#define kdf(OUT, IN, INBYTES) sha256(OUT, IN, INBYTES)


#endif /* SYMMETRIC_H */
