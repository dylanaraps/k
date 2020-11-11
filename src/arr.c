/**
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "arr.h"

void *arr_alloc(void *s, size_t l) {
    size_t *p = s ? arr_raw(s) : 0;
    size_t *n = realloc(p, (sizeof(void *) * (l + 2)));

    if (!n) {
        return NULL;
    }

    n[0] = l;
    n[1] = p ? n[1] : 0;

    return (void *) &n[2];
}

void arr_drop_b(void *s) {
    ((size_t *) (s))[-1] -= 1;
}

void arr_sort(void *s, int (*cb)(const void *, const void *)) {
    qsort(s, arr_len(s), sizeof (void *), cb);
}

void arr_free(void *s) {
    free(s ? arr_raw(s) : 0);
}

