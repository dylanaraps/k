/**
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
 */
#ifndef KISS_CACHE_H
#define KISS_CACHE_H

#include "buf.h"

int cache_init(buf **cache_dir);
int cache_clean(buf *cache_dir);

#endif
