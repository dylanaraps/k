#ifndef KISS_CACHE_H
#define KISS_CACHE_H

#include "str.h"

struct cache {
    str *path;
    int fd[7];
};

enum cache_dir {
    CAC_BIN,
    CAC_SRC,
    CAC_LOG,

    CAC_DIR,
};

struct cache *cache_create(void);
int cache_init(struct cache **cac);
int cache_mkdir(struct cache *cac);
int cache_open_fds(struct cache **cac);
int cache_get_base(str **s);
void cache_free(struct cache **cac);

#endif
