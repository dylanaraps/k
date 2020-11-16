/**
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
 */
#ifndef KISS_ERROR_H
#define KISS_ERROR_H

#include <errno.h>
#include <stdio.h>
#include <string.h>

/**
 * Display an error message with file, function and line number infomation.
 */
#define err(...) do {                           \
    fprintf(stderr, "error: ");                 \
    fprintf(stderr, __VA_ARGS__);               \
    fprintf(stderr, " (%s in %s() at line %d)", \
        __FILE__, __func__, __LINE__);          \
    fputc('\n', stderr);                        \
} while (0)

/**
 * Display an error message with file, function and line number information.
 * Also, append the operating system error via strerror().
 */
#define err_no(...) do {                        \
    fprintf(stderr, "error: ");                 \
    fprintf(stderr, __VA_ARGS__);               \
    fprintf(stderr, ": %s", strerror(errno));   \
    fprintf(stderr, " (%s in %s() at line %d)", \
        __FILE__, __func__, __LINE__);          \
    fputc('\n', stderr);                        \
} while (0)

/**
 * Display a message.
 */
#define msg(...) do {             \
    fprintf(stdout, __VA_ARGS__); \
    fputc('\n', stdout);          \
} while (0)

#endif
