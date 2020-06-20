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
    int pid_len;
    char* pid_str = 0;
    pid_t pid;
    int ret;

    xdg_cache_dir(CAC_DIR, PATH_MAX);
    pid = getpid();
    mkdir_p(CAC_DIR);

    if (chdir(CAC_DIR) != 0) {
        die("Cache directory is not accessible");
    }

    for (int i = 0; i < 3; i++) {
       mkdir_p(caches[i]); 
    }

    pid_len = snprintf(NULL, 0,"%d", pid);
    pid_str = xmalloc(pid_len + 1);
    ret     = snprintf(pid_str, pid_len + 1, "%d", pid);

    if (ret == -1) {
        die("Failed to covert pid to string");
    }

    mkdir_p(pid_str);

    if (chdir(pid_str) != 0) {
        die("Cache directory is not accessible");
    }

    for (int i = 0; i < 3; i++) {
       mkdir_p(states[i]); 
    }
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
