/**
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
 */
#ifndef KISS_SHA256_H
#define KISS_SHA256_H

#include <stdio.h>
#include <stdint.h>

/**
 * Use OpenSSL sha256 if available falling back to a standalone implementation.
 * OpenSSL has ASM implementations of sha256 (which use SSSE3, AVX, whatever)
 * for various architectures. LibreSSL only includes ASM for "major
 * architectures", see https://github.com/q66/libressl-portable-asm.
 */
#ifdef USE_OPENSSL
#include "openssl/sha.h"
#else

#define SHA256_DIGEST_LENGTH 32

typedef struct SHA256state_st {
    uint32_t state[8];
    uint64_t count;
    unsigned char buf[SHA256_DIGEST_LENGTH * 2];
} SHA256_CTX;

void SHA256_Init(SHA256_CTX *c);
void SHA256_Update(SHA256_CTX *c, const void *data, size_t len);
void SHA256_Final(unsigned char *md, SHA256_CTX *c);

#endif

/**
 * Generate checksums for a file. Result is stored in hash.
 */
void sha256_file(unsigned char hash[SHA256_DIGEST_LENGTH], FILE *f);

/**
 * Convert hash to its string representation. Result is stored in out.
 */
void sha256_to_string(unsigned char hash[SHA256_DIGEST_LENGTH], char out[65]);

#endif

