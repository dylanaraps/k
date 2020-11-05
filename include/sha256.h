/**
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
 */
#ifndef KISS_SHA256_H
#define KISS_SHA256_H

/**
 * Use OpenSSL sha256 if available falling back to a standalone implementation.
 * OpenSSL has ASM implementations of sha256 (which use SSSE3, AVX, whatever)
 * for various architectures. LibreSSL only includes ASM for "major
 * architectures", see https://github.com/q66/libressl-portable-asm.
 */
#ifdef USE_OPENSSL
#include "openssl/sha.h"
#else

// todo

#endif

#endif

