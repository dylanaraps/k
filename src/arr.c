/**
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "arr.h"

void *arr_alloc(void *a, size_t l) {
    size_t *p = a ? arr_raw(a) : 0;
    size_t *n = realloc(p, (sizeof(void *) * (l + 2)));

    if (!n) {
        return NULL;
    }

    n[0] = l;
    n[1] = p ? n[1] : 0;

    return (void *) &n[2];
}

inline void arr_drop_b(void *a) {
    arr_rem_len(a, 1);
}

inline void arr_sort(void *a, int (*cb)(const void *, const void *)) {
    qsort(a, arr_len(a), sizeof(void *), cb);
}

inline void arr_free(void *a) {
    free(a ? arr_raw(a) : 0);
}

