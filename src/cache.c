#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "str.h"
#include "util.h"
#include "cache.h"

int cache_init(str **cache_dir) {
    if (cache_get_base(cache_dir) != 0) {
        return -1;
    }

    str_printf(cache_dir, "/proc/%u/", getpid());

    if ((*cache_dir)->err != STR_OK) {
        err("string error");
        return -1;
    }

    if (mkdir_p((*cache_dir)->buf) != 0) {
        err("failed to create directory '%s': %s",
            (*cache_dir)->buf, strerror(errno));
        return -1;
    }

    return 0;
}

int cache_mkdir(str *cache_dir) {
    static const char *caches[]  = {
        "build", 
        "extract", 
        "pkg", 
        "../../sources", 
        "../../bin"
    };

    int cache_fd = open(cache_dir->buf, O_RDONLY);

    if (cache_fd == -1) {
        err("failed to open cache directory: %s", strerror(errno));
        return -1;
    }

    for (size_t i = 0; i < (sizeof caches / sizeof caches[0]); i++) {
        if (mkdirat(cache_fd, caches[i], 0755) == -1 && errno != EEXIST) {
            err("failed to create cache directory '%s': %s", 
                caches[i], strerror(errno));
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
            err("HOME is unset");
            return -1;
        }

        str_rstrip(s, '/');
        str_push_l(s, "/.cache", 7);
    }

    if ((*s)->buf[0] != '/') {
        err("cache directory not absolute");
        return -1;
    }

    str_rstrip(s, '/');
    str_push_l(s, "/kiss", 5);
    return 0;
}
