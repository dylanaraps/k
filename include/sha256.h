/**
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
 */
#ifndef KISS_SHA256_H
#define KISS_SHA256_H

#include <stdio.h>
#include <stdint.h>

#define SHA256_LEN 32

/**
 * Use OpenSSL sha256 if available falling back to a standalone implementation.
 * OpenSSL has ASM implementations of sha256 (which use SSSE3, AVX, whatever)
 * for various architectures.
 */
#if defined(USE_OPENSSL)
#include <openssl/sha.h>

/**
 * Use bearssl sha256 if available falling back to a standalone implementation.
 * (This is the fastest implementation in my testing).
 */
#elif defined(USE_BEARSSL)
#include <bearssl_hash.h>

typedef br_sha256_context SHA256_CTX;

#define SHA256_Init(c)         br_sha256_init(c)
#define SHA256_Update(c, d, l) br_sha256_update(c, d, l)
#define SHA256_Final(md, c)    br_sha256_out(c, md)

/**
 * Fallback internal sha256 implementation.
 */
#else

typedef struct SHA256state_st {
    uint32_t state[8];
    uint64_t count;
    unsigned char buf[SHA256_LEN * 2];
} SHA256_CTX;

void SHA256_Init(SHA256_CTX *c);
void SHA256_Update(SHA256_CTX *c, const void *data, size_t len);
void SHA256_Final(unsigned char *md, SHA256_CTX *c);

#endif

/**
 * Generate checksums for a file. Result is stored in hash.
 */
void sha256_file(unsigned char hash[SHA256_LEN], FILE *f);

/**
 * Convert hash to its string representation. Result is stored in out.
 */
void sha256_to_string(unsigned char hash[SHA256_LEN], char out[65]);

#endif

