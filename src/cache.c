#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "buf.h"
#include "file.h"
#include "error.h"
#include "cache.h"

static const char *caches[] = {
    "build",
    "pkg",
    "extract",

    "../../sources",
    "../../logs",
    "../../bin",
};

int cache_init(struct cache *c) {
    c->dir = buf_alloc(0, 256);

    if (!c->dir) {
        return -ENOMEM;
    }

    if (cache_get_base(&c->dir) < 0) {
        return -1;
    }

    if (cache_init_all(c) < 0) {
        return -1;
    }

    return 0;
}

int cache_init_all(struct cache *c) {
    if (mkdir_p(c->dir, 0755) < 0) {
        return -1;
    }

    if ((c->fd[6] = open(c->dir, O_RDONLY)) == -1) {
        err_no("failed to open directory '%s'", c->dir);
        return -1;
    }

    for (size_t i = 0; i < CAC_DIR; i++) {
        if (mkdirat(c->fd[6], caches[i], 0755) == -1 && errno != EEXIST) {
            err_no("failed to create directory '%s%s'", c->dir, caches[i]);
            return -1;
        }

        if ((c->fd[i] = openat(c->fd[6], caches[i], O_RDONLY)) == -1) {
            err_no("failed to open directory '%s%s'", c->dir, caches[i]);
            return -1;
        }
    }

    return 0;
}

int cache_get_base(buf **c) {
    switch(buf_push_s(c, getenv("XDG_CACHE_HOME"))) {
        case -ENOMEM:
            return -ENOMEM;

        case -EINVAL:
            switch (buf_push_s(c, getenv("HOME"))) {
                case -ENOMEM:
                    return -ENOMEM;

                case -EINVAL:
                    err("XDG_CACHE_HOME and HOME unset");
                    return -1;
            }

            buf_rstrip(c, '/');
            buf_push_l(c, "/.cache", 7);
    }

    buf_rstrip(c, '/');

    return buf_printf(c, "/kiss/proc/%ld/", (long) getpid());
}

int cache_clean(struct cache *c) {
    return rm_rf(c->dir);
}

void cache_free(struct cache *c) {
    for (size_t i = 0; i < CAC_DIR; i++) {
        close(c->fd[i]);
    }

    buf_free(&c->dir);
}

