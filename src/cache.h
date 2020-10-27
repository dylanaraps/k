#ifndef KISS_CACHE_H
#define KISS_CACHE_H

#include "str.h"

int cache_init(str **cache_dir);
int cache_get_base(str **s);
int cache_mkdir(str *cache_dir);

#endif
