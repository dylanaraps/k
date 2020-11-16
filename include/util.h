/**
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
 */
#ifndef KISS_UTIL_H
#define KISS_UTIL_H

#include <stdint.h>

/**
 * Turn bytes into human readable representation.
 */
char *human_readable(uint64_t n, char out[6]);

#endif
