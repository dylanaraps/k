#ifndef KISS_CACHE_H_
#define KISS_CACHE_H_

#include "str.h"

void cache_init(void);
void cache_free(void);
char *get_cache_dir(void);

#endif
