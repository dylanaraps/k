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
#define arr_alloc_maybe(a, l) do {               \
    if ((arr_len(a) + l) > arr_cap(a)) {         \
        void *_n = arr_alloc(a, arr_inc_cap(a)); \
        assert(_n);                              \
        (a) = _n;                                \
    }                                            \
} while(0)

/**
 * Push an element to the list, growing it by a factor of 1.5 (if needed).
 */
#define arr_push_b(a, d) do { \
    arr_alloc_maybe(a, 1);    \
    arr_set_end(a, d);        \
    arr_add_len(a, 1);        \
} while(0)

/**
 * Free the array (caller must free elements).
 */
#define arr_free(a)       free(a ? arr_raw(a) : 0)

/**
 * Sort array using qsort. cb equates to the function pointer
 * given to qsort: int (*cb)(const void *, const void *)
 */
#define arr_sort(a, cb)   qsort(a, arr_len(a), sizeof(void *), cb)

/**
 * Drop element from back of array.
 */
#define arr_drop_b(a)     arr_rem_len(a, 1)

/**
 * Various macros to ease data accesss.
 */
#define arr_raw(a)        ((size_t *) (a) - 2)
#define arr_len(a)        (arr_raw(a)[1])
#define arr_cap(a)        (arr_raw(a)[0])
#define arr_set_end(a, d) ((a)[arr_len(a)] = (d))
#define arr_add_len(a, l) (arr_len(a) += (l))
#define arr_rem_len(a, l) (arr_len(a) -= (l))
#define arr_inc_cap(a)    (arr_cap(a) + (arr_cap(a) >> 1))

#endif

