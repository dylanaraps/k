#include <errno.h>

#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "str.h"
#include "util.h"
#include "cache.h"

struct cache *cache_create(void) {
    struct cache *c = malloc(sizeof *c);

    if (c) {
        c->fd   = 0;
        c->path = str_init(256);

        if (c->path) {
            return c;
        }

        free(c);
    }

    return NULL;
}

int cache_init(struct cache **cac) {
    if (cache_get_base(&(*cac)->path) < 0) {
        return -1;
    }

    if (mkdir_p((*cac)->path) < 0) {
        err_no("failed to create directory '%s'", (*cac)->path);
        return -1;
    }

    (*cac)->fd = open((*cac)->path, O_RDONLY);

    if ((*cac)->fd < 0) {
        err_no("failed to open cache directory '%s'", (*cac)->path);
        return -1;
    }

    return cache_mkdir(*cac);
}

int cache_mkdir(struct cache *cac) {
    static const char *caches[]  = {
        "build", 
        "extract", 
        "pkg", 
        "../../sources", 
        "../../bin"
    };

    for (size_t i = 0; i < (sizeof caches / sizeof caches[0]); i++) {
        if (mkdirat(cac->fd, caches[i], 0755) == -1 && errno != EEXIST) {
            err_no("failed to create cache directory '%s'", caches[i]);
            return -1;
        }
    }

    return 0;
}

int cache_get_base(str **s) {
    if (str_push_s(s, getenv("XDG_CACHE_HOME")) < 0) {
        if (str_push_s(s, getenv("HOME")) < 0) {
            err("HOME and XDG_CACHE_HOME are unset");
            return -1;
        }

        if (str_push_l(s, "/.cache", 7) < 0) {
            err("string error");
            return -1;
        }
    }

    if (str_printf(s, "/kiss/proc/%ld/", (long) getpid()) < 0) {
        err("string error");
        return -1;
    }

    return 0;
}

void cache_free(struct cache **cac) {
    str_free(&(*cac)->path);
    close((*cac)->fd);
    free(*cac);
    *cac = NULL;
}

