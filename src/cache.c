#define _POSIX_C_SOURCE 200809L
#include <stdlib.h> /* size_t */
#include <stdio.h>  /* snprintf */
#include <unistd.h> /* getenv */
#include <limits.h> /* PATH_MAX */
#include <string.h> /* strchr */
#include <ftw.h>    /* ntfw */

#include "log.h"
#include "pkg.h"
#include "util.h"
#include "file.h"
#include "cache.h"

char CAC_DIR[PATH_MAX];

static const char *caches[] = {
    "sources",
    "bin",
    "logs",
};
static const int cache_len = 3;

static const char *states[] = {
    "build",
    "extract",
    "pkg",
};
static const int state_len = 3;

void cache_init(void) {
    char xdg[PATH_MAX];
    pid_t pid = getpid();
    int ret;

    xdg_cache_dir(xdg, PATH_MAX);
    xsnprintf(CAC_DIR, PATH_MAX, "%s/%ul", xdg, pid);

    mkdir_p(CAC_DIR);

    if (chdir(CAC_DIR) != 0) {
        die("Cache directory is not accessible");
    }

    for (ret = 0; ret < state_len; ret++) {
        mkdir_p(states[ret]);
    }

    /* drop PID portion */
    if (chdir("..") != 0) {
        die("Cache directory is not accessible");
    }

    for (ret = 0; ret < cache_len; ret++) {
        mkdir_p(caches[ret]);
    }
}

void state_init(package *pkg, const char *type, char *buf) {
    char *tmp;

    if (chdir(CAC_DIR) != 0) {
        die("[%s] Cache directory is not accessible", pkg->name);
    }

    if (chdir(type) != 0) {
        die("[%s] Cache directory (%s) is not accessible", type, pkg->name);
    }

    mkdir_p(pkg->name);

    if (chdir(pkg->name) != 0) {
        die("[%s] Cache directory (%s) is not accessible", type, pkg->name);
    }

    tmp = getcwd(buf, PATH_MAX);

    if (!tmp) {
        die("[%s] Failed to init cache directory %s", type, pkg->name);
    }
}


void xdg_cache_dir(char *buf, int len) {
    char *dir;

    dir = getenv("XDG_CACHE_HOME");

    if (!dir || !len) {
        die("Failed to construct cache directory");
    }

    if (!dir) {
        dir = getenv("HOME");

        if (!dir) {
            die("HOME is NULL");
        }

        xsnprintf(buf, len, "%s/.cache/kiss", dir);

        return;
    }

    if (!strchr(dir, '/')) {
        die("Invalid XDG_CACHE_HOME");
    }

    xsnprintf(buf, len, "%s/kiss", dir);
}

void cache_destroy(void) {
    rm_dir(CAC_DIR);
}
