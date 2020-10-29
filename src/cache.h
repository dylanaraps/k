#ifndef KISS_CACHE_H
#define KISS_CACHE_H

#include "str.h"

struct cache {
    str *path;
    int fd;
};

struct cache *cache_create(void);
int cache_init(struct cache **cac);
int cache_mkdir(struct cache *cac);
void cache_free(struct cache **cac);

#endif
