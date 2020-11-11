/**
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
 */
#ifndef KISS_ARR_H
#define KISS_ARR_H

#include <assert.h>
#include <stdlib.h>

/**
 * Initialize array and allocate memory for s elements. If s is 0, no additional
 * memory is allocated. Returns NULL when memory allocation fails.
 */
void *arr_alloc(void *s, size_t l);

/**
 * Grow the array if l additional items do not fit.
 */
#define arr_alloc_maybe(s, l) do {            \
    if ((arr_len(s) + l) > arr_cap(s)) {      \
        void *_n = arr_alloc(s,               \
            arr_cap(s) + (arr_cap(s) >> 1));  \
                                              \
        assert(_n);                           \
        (s) = _n;                             \
    }                                         \
} while(0);

/**
 * Push an element to the list, growing it by a factor of 1.5 (if needed).
 */
#define arr_push_b(s, d) do {  \
    arr_alloc_maybe(s, 1)      \
    (s)[arr_len(s)] = (d);     \
    ((size_t *) (s))[-1] += 1; \
} while(0)

/**
 * Drop an element from the end of the list.
 */
void arr_drop_b(void *s);

/**
 * Sort a list using qsort().
 */
void arr_sort(void *s, int (*cb)(const void *, const void *));

/**
 * Free memory associated with the list. Checks for NULL before calling
 * free(). List elements themselves must be freed by the caller.
 */
void arr_free(void *s);

/**
 * Get pointer to beginning of memory allocation.
 */
#define arr_raw(s) (((size_t *) (s) - 2))

/**
 * Get the length of array.
 */
#define arr_len(s) arr_raw(s)[1]

/**
 * Get the capacity (elements allocated) of array.
 */
#define arr_cap(s) arr_raw(s)[0]

#endif
