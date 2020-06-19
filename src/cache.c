#define _POSIX_C_SOURCE 200809L
#include <stdlib.h> /* size_t */
#include <stdio.h>  /* snprintf */
#include <unistd.h> /* getenv */
#include <limits.h> /* PATH_MAX */
#include <string.h> /* strlen, strchr */

#include "log.h"
#include "util.h"
#include "cache.h"

char CAC_DIR[PATH_MAX];

const char *caches[] = {
    "sources", 
    "bin", 
    "logs", 
};

const char *states[] = {
    "build", 
    "pkg", 
    "extract", 
};

void cache_init(void) {
    pid_t pid;

    xdg_cache_dir(CAC_DIR, PATH_MAX);

    pid = getpid();

    mkdir_p(CAC_DIR);
    printf("%s\n", CAC_DIR);
}

void xdg_cache_dir(char *buf, size_t len) {
    char *dir ;
    int err;
    size_t str_len;

    dir = getenv("XDG_CACHE_HOME");

    if (!dir) {
        dir = getenv("HOME"); 

        if (!dir) {
            die("HOME is NULL"); 
        }

        err = snprintf(buf, len, "%s/.cache/kiss", dir);

        if (err == -1) {
            die("Failed to construct cache directory");
        }

        return;
    }

    str_len = strlen(dir);

    if (!strchr(dir, '/')) {
        die("Invalid XDG_CACHE_HOME");
    }

    if (str_len > PATH_MAX) {
        die("XDG_CACHE_HOME exceeds PATH_MAX");
    }

    err = snprintf(buf, len, "%s/kiss", dir);

    if (err == -1) {
        die("Failed to construct cache directory");
    }
}

void cache_destroy(void) {
    
}
