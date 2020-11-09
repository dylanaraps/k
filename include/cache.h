/**
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
 */
#ifndef KISS_CACHE_H
#define KISS_CACHE_H

#include "str.h"

int cache_init(str **cache_dir);
int cache_clean(str *cache_dir);

#endif
