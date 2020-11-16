#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "sha256.h"

void sha256_file(unsigned char hash[SHA256_LEN], FILE *f) {
    unsigned char buf[BUFSIZ];

    SHA256_CTX ctx;
    SHA256_Init(&ctx);

    for (size_t n = 0; (n = fread(buf, 1, BUFSIZ, f)); ) {
        SHA256_Update(&ctx, buf, n);
    }

    SHA256_Final(hash, &ctx);
}

void sha256_to_string(unsigned char hash[SHA256_LEN], char out[65]) {
    for (size_t i = 0; i < SHA256_LEN; i++) {
        snprintf(out + (i * 2), 65, "%02x", hash[i]);
    }

    out[64] = 0;
}

/**
 * Fallback internal sha256 implementation.
 * Modified to match OpenSSL's sha256 functions.
 *
 * 2010-06-11 : Igor Pavlov : Public domain
 * This code is based on public domain code from Wei Dai's Crypto++ library.
 */
#ifndef SHA256_USE_OPENSSL
#ifndef SHA256_USE_BEARSSL

void SHA256_Init(SHA256_CTX *c) {
    c->count = 0;

    c->state[0] = 0x6a09e667;
    c->state[1] = 0xbb67ae85;
    c->state[2] = 0x3c6ef372;
    c->state[3] = 0xa54ff53a;
    c->state[4] = 0x510e527f;
    c->state[5] = 0x9b05688c;
    c->state[6] = 0x1f83d9ab;
    c->state[7] = 0x5be0cd19;
}

#define rotrFixed(x, n) (((x) >> (n)) | ((x) << (32 - (n))))
#define S0(x) (rotrFixed(x, 2) ^ rotrFixed(x,13) ^ rotrFixed(x, 22))
#define S1(x) (rotrFixed(x, 6) ^ rotrFixed(x,11) ^ rotrFixed(x, 25))
#define s0(x) (rotrFixed(x, 7) ^ rotrFixed(x,18) ^ (x >> 3))
#define s1(x) (rotrFixed(x,17) ^ rotrFixed(x,19) ^ (x >> 10))

#define blk0(i) (W[i] = data[i])
#define blk2(i) (W[(i)&15] += s1(W[(i-2)&15]) + W[(i-7)&15] + s0(W[(i-15)&15]))

#define Ch(x,y,z) (z^(x&(y^z)))
#define Maj(x,y,z) ((x&y)|(z&(x|y)))

#define a(i) T[(0-(i))&7]
#define b(i) T[(1-(i))&7]
#define c(i) T[(2-(i))&7]
#define d(i) T[(3-(i))&7]
#define e(i) T[(4-(i))&7]
#define f(i) T[(5-(i))&7]
#define g(i) T[(6-(i))&7]
#define h(i) T[(7-(i))&7]

#define R(a,b,c,d,e,f,g,h, i) h += S1(e) + Ch(e,f,g) + K[i+j] + \
    (j?blk2(i):blk0(i)); d += h; h += S0(a) + Maj(a, b, c)

#define RX_8(i) \
    R(a,b,c,d,e,f,g,h, i);   \
    R(h,a,b,c,d,e,f,g, i+1); \
    R(g,h,a,b,c,d,e,f, i+2); \
    R(f,g,h,a,b,c,d,e, i+3); \
    R(e,f,g,h,a,b,c,d, i+4); \
    R(d,e,f,g,h,a,b,c, i+5); \
    R(c,d,e,f,g,h,a,b, i+6); \
    R(b,c,d,e,f,g,h,a, i+7)

static const uint32_t K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

static void sha256_transform(uint32_t *state, const uint32_t *data) {
    uint32_t W[16] = {0};

    uint32_t a = state[0];
    uint32_t b = state[1];
    uint32_t c = state[2];
    uint32_t d = state[3];
    uint32_t e = state[4];
    uint32_t f = state[5];
    uint32_t g = state[6];
    uint32_t h = state[7];

    unsigned j = 0;
    RX_8(0);
    RX_8(8);
    j += 16;
    RX_8(0);
    RX_8(8);
    j += 16;
    RX_8(0);
    RX_8(8);
    j += 16;
    RX_8(0);
    RX_8(8);

#undef S0
#undef S1
#undef s0
#undef s1

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
    state[5] += f;
    state[6] += g;
    state[7] += h;
}

static void sha256_write(SHA256_CTX *c) {
    uint32_t data[16];

    for (unsigned i = 0; i < 16; i++) {
        data[i] =
            ((uint32_t)(c->buf[i * 4    ]) << 24) +
            ((uint32_t)(c->buf[i * 4 + 1]) << 16) +
            ((uint32_t)(c->buf[i * 4 + 2]) <<  8) +
            ((uint32_t)(c->buf[i * 4 + 3]));
    }

    sha256_transform(c->state, data);
}

void SHA256_Update(SHA256_CTX *c, const void *data, size_t size) {
    const unsigned char *d = data;
    uint32_t pos = c->count & 0x3F;

    while (size > 0) {
        c->buf[pos++] = *d++;
        c->count++;
        size--;

        if (pos == 64) {
            pos = 0;
            sha256_write(c);
        }
    }
}

void SHA256_Final(unsigned char *md, SHA256_CTX *c) {
    uint64_t len_bits = (c->count << 3);
    uint32_t pos = c->count & 0x3F;

    c->buf[pos++] = 0x80;

    while (pos != (64 - 8)) {
        pos &= 0x3F;

        if (pos == 0) {
            sha256_write(c);
        }

        c->buf[pos++] = 0;
    }

    for (unsigned i = 0; i < 8; i++) {
        c->buf[pos++] = (len_bits >> 56);
        len_bits <<= 8;
    }

    sha256_write(c);

    for (unsigned i = 0; i < 8; i++) {
        *md++ = (c->state[i] >> 24);
        *md++ = (c->state[i] >> 16);
        *md++ = (c->state[i] >> 8);
        *md++ = (c->state[i]);
    }
}

#endif
#endif

