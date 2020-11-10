#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include "str.h"
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

static const size_t cache_len[] = {
    5,  3, 7,
   13, 10, 9,
};

static int cache_get_base(str **s) {
    if (str_push_s(s, getenv("XDG_CACHE_HOME")) < 0) {
        if (str_push_s(s, getenv("HOME")) < 0) {
            return -1;
        }

        str_rstrip(s, '/');

        if (str_push_l(s, "/.cache", 7) < 0) {
            return -1;
        }
    }

    str_rstrip(s, '/');
    return str_printf(s, "/kiss/proc/%ld/", getpid());
}

int cache_init(str **cache_dir) {
    if (cache_get_base(cache_dir) < 0) {
        return -1;
    }

    if (mkdir_p(*cache_dir, 0755) < 0) {
        return -1;
    }

    for (size_t i = 0; i < sizeof(caches) / sizeof(caches[0]); i++) {
        str_push_l(cache_dir, caches[i], cache_len[i]);

        if (mkdir(*cache_dir, 0755) == -1 && errno != EEXIST) {
            err_no("failed to create directory '%s'", *cache_dir);
            return -1;
        }

        str_set_len(cache_dir, str_len(cache_dir) - cache_len[i]);
    }

    return 0;
}

int cache_clean(str *cache_dir) {
    return rm_rf(cache_dir);
}

