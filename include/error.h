/**
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
 */
#ifndef KISS_ERROR_H
#define KISS_ERROR_H

#include <errno.h>
#include <stdio.h>
#include <string.h>

#define msg(...) do {             \
    fprintf(stderr, __VA_ARGS__); \
    fputc('\n', stderr);          \
} while (0)

#define err(...) do {                           \
    fprintf(stderr, "error: ");                 \
    fprintf(stderr, __VA_ARGS__);               \
    fprintf(stderr, " (%s in %s() at line %d)", \
        __FILE__, __func__, __LINE__);          \
    fputc('\n', stderr);                        \
} while (0)

#define err_no(...) do {                        \
    fprintf(stderr, "error: ");                 \
    fprintf(stderr, __VA_ARGS__);               \
    fprintf(stderr, ": %s", strerror(errno));   \
    fprintf(stderr, " (%s in %s() at line %d)", \
        __FILE__, __func__, __LINE__);          \
    fputc('\n', stderr);                        \
} while (0)

#endif
