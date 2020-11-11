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
void *arr_alloc(void *a, size_t l);

/**
 * Grow the array if l additional items do not fit.
 */
#define arr_alloc_maybe(a, l) do {            \
    if ((arr_len(a) + l) > arr_cap(a)) {      \
        void *_n = arr_alloc(a,               \
            arr_cap(a) + (arr_cap(a) >> 1));  \
                                              \
        assert(_n);                           \
        (a) = _n;                             \
    }                                         \
} while(0);

/**
 * Push an element to the list, growing it by a factor of 1.5 (if needed).
 */
#define arr_push_b(a, d) do {  \
    arr_alloc_maybe(a, 1)      \
    (a)[arr_len(a)] = (d);     \
    ((size_t *) (a))[-1] += 1; \
} while(0)

/**
 * Drop an element from the end of the list.
 */
void arr_drop_b(void *a);

/**
 * Sort a list using qsort().
 */
void arr_sort(void *a, int (*cb)(const void *, const void *));

/**
 * Free memory associated with the list. Checks for NULL before calling
 * free(). List elements themselves must be freed by the caller.
 */
void arr_free(void *a);

/**
 * Get pointer to beginning of memory allocation.
 */
#define arr_raw(a) (((size_t *) (a) - 2))

/**
 * Get the length of array.
 */
#define arr_len(a) arr_raw(a)[1]

/**
 * Get the capacity (elements allocated) of array.
 */
#define arr_cap(a) arr_raw(a)[0]

#endif
