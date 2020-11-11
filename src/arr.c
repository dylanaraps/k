/**
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
 */
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

