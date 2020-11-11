/**
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
 */
#ifndef KISS_CACHE_H
#define KISS_CACHE_H

#include "buf.h"

struct cache {
    buf *dir;
    int fd[7];
};

enum cache_type {
    CAC_MAK,
    CAC_PKG,
    CAC_EXT,

    CAC_SRC,
    CAC_LOG,
    CAC_BIN,

    CAC_DIR,
};

int cache_init(struct cache *c);
int cache_init_all(struct cache *c);
int cache_get_base(buf **c);
int cache_clean(struct cache *c);
void cache_free(struct cache *c);

#endif
