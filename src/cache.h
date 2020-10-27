#ifndef KISS_CACHE_H
#define KISS_CACHE_H

#include "str.h"

int cache_init(void);
int cache_create(void);
void cache_free(void);

int cache_get_base(str **s);

#endif
