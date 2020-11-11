/**
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
 */
#ifndef KISS_ARR_H
#define KISS_ARR_H

#include <assert.h>
#include <stdlib.h>

/**
 * Allocate or grow array by l bytes. If a is NULL, memory will be allocted
 * for the array. If a is not NULL, the array will grow by l elements. Returns
 * NULL if memory allocation fails and a is unmodified.
 *
 * Functions will call this internally for you. This is only really needed
 * during array creation.
 *
 * // create array, don't allocate any extra memory for elements.
 * char **new = arr_alloc(0, 0);
 *
 * // create array and preallocate memory for 20 elements.
 * char **new = buf_alloc(0, 20);
 *
 */
void *arr_alloc(void *a, size_t l);

/**
 * Grow the array if l additional items do not fit.
 */
#define arr_alloc_maybe(a, l) do {            \
    if ((arr_len(a) + l) > arr_cap(a)) {      \
        void *_n = arr_alloc(a,               \
            arr_cap(a) + (arr_cap(a) >> 1));  \
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
