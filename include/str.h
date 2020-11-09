/**
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
 *
 * Notes:
 *
 * - Memory is overallocated by a factor of 1.5 to prevent growing the
 *   buffer every time new data is pushed to it. Memory can also be
 *   preallocated during str_init(). The character array is a flexible
 *   struct member so (re)allocation only needs a single function call.
 *
 * - Functions which push data always append to the end of any existing data.
 *   This behavior can be controlled via str_set_len() as the value of the
 *   len struct member is where pushes start. This function will also NULL
 *   terminate the string at the new length.
 *
 * - All functions which cause buffer growth return -ENOMEM on allocation
 *   failure. str_init() returns with NULL on allocation failure. str_push_s()
 *   returns with -EINVAL if input is NULL.
 *
 * - The buffer is null terminated so it can be used with regular functions.
 *   Access is via the buf struct member.
 *
 */
#ifndef KISS_STR_H
#define KISS_STR_H

#include <stdlib.h>
#include <stdio.h>

typedef struct {
    size_t cap;
    size_t len;
    char buf[];
} str;

/**
 * Allocate l bytes of memory for the string and initialize all struct fields
 * to their respective values. Returns NULL on failure. This function must be
 * called before any other str_ functions can be used.
 */
str *str_init(size_t l);

/**
 * Free all memory associated with the string. Checks for NULL before calling
 * free().
 */
void str_free(str **s);

/**
 * Push a string of known length. Returns 0 for success and -ENOMEM if memory
 * allocation fails.
 */
int str_push_l(str **s, const char *d, size_t l);

/**
 * Push a string of unknown length. Returns 0 for success, -EINVAL if d is
 * NULL and -ENOMEM if memory allocation fails. Wrapper around str_push_l()
 * using strlen(d) to determine length of input.
 */
int str_push_s(str **s, const char *d);

/**
 * Push a character. Returns 0 for success, -ENOMEM if memory allocation fails.
 */
int str_push_c(str **s, int d);

/**
 * Drop a character from the end of the string if it matches d. Returns 0 for
 * success and -1 for failure.
 */
int str_undo_c(str **s, int d);

/**
 * Drop every character matching d from the end of the string. Returns how
 * many characters were dropped. 0 denotes failure.
 */
int str_rstrip(str **s, int d);

/**
 * Push the next line in f to the string in chunks of l. No additional memory
 * is allocated for each line read (unless the buffer must be grown). The
 * trailing newline is removed if present. Returns 0 for success and -1 for
 * failure.
 */
int str_getline(str **s, FILE *f, size_t l);

/**
 * Wrapper around vsnprintf() which automatically grows buffer if needed.
 * Returns 0 on success and -ENOMEM or -1 on failure.
 */
int str_printf(str **s, const char *f, ...);

/**
 * Change the string's length to l and set the corresponding position in the
 * string to '\0'. This can be used to safely (and cheaply) truncate strings.
 */
#define str_set_len(s, l) (((s)->buf)[((s)->len) = (l)] = 0)

#endif
