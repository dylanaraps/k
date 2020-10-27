#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "str.h"
#include "util.h"
#include "cache.h"

// Holds the cache directory + /proc/ + <pid>.
static str *cache_dir = 0;

int cache_init(void) {
    cache_dir = str_init(256);    

    if (!cache_dir) {
        return -4;
    }

    if (cache_get_base(&cache_dir) != 0) {
        return -3;
    }

    str_printf(&cache_dir, "/proc/%u/", getpid());

    if (cache_dir->err != STR_OK) {
        return -2;
    }

    if (mkdir_p(cache_dir->buf) != 0) {
        return -1;
    }

    return cache_create();
}

int cache_create(void) {
    static const char *caches[5]  = {
        "build", 
        "extract", 
        "pkg", 
        "../../sources", 
        "../../bin"
    };

    int cache_fd = open(cache_dir->buf, O_RDONLY);

    for (size_t i = 0; i < 5; i++) {
        if (mkdirat(cache_fd, caches[i], 0755) == -1 && errno != EEXIST) {
            return -1;
        }
    }

    return close(cache_fd);
}

int cache_get_base(str **s) {
    str_push_s(s, getenv("XDG_CACHE_HOME"));

    if ((*s)->err != STR_OK) {
        (*s)->err = STR_OK;

        str_push_s(s, getenv("HOME"));

        if ((*s)->err != STR_OK) {
            return -1;
        }

        str_rstrip(s, '/');
        str_push_l(s, "/.cache", 7);
    }

    if ((*s)->buf[0] != '/') {
        return -1;
    }

    str_rstrip(&cache_dir, '/');
    str_push_l(s, "/kiss", 5);
    return 0;
}

void cache_free(void) {
    str_free(&cache_dir);
}

