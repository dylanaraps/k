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
        c->path = 0;
        c->fd = 0;
    }

    return c;
}

int cache_init(struct cache **cac) {
    if (str_push_s(&(*cac)->path, getenv("XDG_CACHE_HOME")) < 0) {
        if (str_push_s(&(*cac)->path, getenv("HOME")) < 0) {
            err("HOME and XDG_CACHE_HOME are unset");
            return -1;
        }

        if (str_push_l(&(*cac)->path, "/.cache", 7) < 0) {
            return -1;
        }
    }

    if (str_printf(&(*cac)->path, "/kiss/proc/%ld/", (long) getpid()) < 0) {
        return -1;
    }

    if (mkdir_p((*cac)->path) != 0) {
        err("failed to create directory '%s': %s", 
            (*cac)->path, strerror(errno));
        return -1;
    }

    (*cac)->fd = open((*cac)->path, O_RDONLY);

    if ((*cac)->fd == -1) {
        err("failed to open cache directory '%s': %s", 
            (*cac)->path, strerror(errno));
        return -1;
    }

    return 0;
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
            err("failed to create cache directory '%s': %s", 
                caches[i], strerror(errno));
            return -1;
        }
    }

    return 0;
}

void cache_free(struct cache **cac) {
    str_free(&(*cac)->path);
    close((*cac)->fd);
    free(*cac);
    *cac = NULL;
}

