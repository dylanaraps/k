/**
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
 */
#ifndef KISS_buf_H
#define KISS_buf_H

#include <stdlib.h>
#include <stdio.h>

typedef char buf;

/**
 * Allocate or grow a string by l bytes. If s is NULL, memory will be allocted
 * for the string. If s is not NULL, the string will grow by l bytes. Returns
 * NULL if memory allocation fails and string is unmodified.
 *
 * Functions will call this internally for you. This is only really needed
 * during string creation.
 *
 * // create string, don't allocate any extra memory for char array.
 * buf *new = buf_alloc(0, 0);
 *
 * // create string and preallocate 20 bytes of memory for char array.
 * buf *new = buf_alloc(0, 20);
 *
 */
buf *buf_alloc(buf **s, size_t l);

/**
 * Free all memory associated with the string. Checks for NULL before calling
 * free().
 */
void buf_free(buf **s);

/**
 * Push a string of known length. Returns 0 for success and -ENOMEM if memory
 * allocation fails.
 */
int buf_push_l(buf **s, const char *d, size_t l);

/**
 * Push a string of unknown length. Returns 0 for success, -EINVAL if d is
 * NULL and -ENOMEM if memory allocation fails. Wrapper around buf_push_l()
 * using buflen(d) to determine length of input.
 */
int buf_push_s(buf **s, const char *d);

/**
 * Push a character. Returns 0 for success, -ENOMEM if memory allocation fails.
 */
int buf_push_c(buf **s, int d);

/**
 * Drop a character from the end of the string if it matches d. Returns 0 for
 * success and -1 for failure.
 */
int buf_undo_c(buf **s, int d);

/**
 * Drop every character matching d from the end of the string.
 */
void buf_rstrip(buf **s, int d);

/**
 * Push the next line in f to the string in chunks of l. No additional memory
 * is allocated for each line read (unless the buffer must be grown). The
 * trailing newline is removed if present. Returns 0 for success and -1 for
 * failure.
 */
int buf_getline(buf **s, FILE *f, size_t l);

/**
 * Wrapper around vsnprintf() which automatically grows buffer if needed.
 * Returns 0 on success and -ENOMEM or -1 on failure.
 */
int buf_printf(buf **s, const char *f, ...);

/**
 * Wrapper around memset() which automatically grows buffer if needed.
 * Returns 0 on success and -ENOMEM on failure.
 */
int buf_set(buf **s, int c, size_t l);

/**
 * Find first occurance of c, zero this position in the string and return
 * pos + 1. Can be used as a quick tokenizer. Length is not updated.
 */
size_t buf_scan(buf **s, int c);

/**
 * Get pointer to beginning of memory allocation.
 */
#define buf_raw(s) ((s) ? (*s) ? ((size_t *) (*s) - 2) : 0 : 0)

/**
 * Get the length of a string.
 */
#define buf_len(s) (((size_t *) (s))[-1])

/**
 * Get the capacity (memory allocated) of a string.
 */
#define buf_cap(s) (((size_t *) (s))[-2])

/**
 * Last character in string.
 */
#define buf_end(s) ((s)[buf_len(s) - 1])

/**
 * Change the string's length to l and set the corresponding position in the
 * string to '\0'. This can be used to safely (and cheaply) truncate strings.
 */
#define buf_set_len(s, l) (((s)[buf_len(s) = (l)]) = 0)

#endif
