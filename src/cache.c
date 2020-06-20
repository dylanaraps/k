#define _POSIX_C_SOURCE 200809L
#include <stdlib.h> /* size_t */
#include <stdio.h>  /* snprintf */
#include <unistd.h> /* getenv */
#include <limits.h> /* PATH_MAX */
#include <string.h> /* strlen, strchr */
#include <ftw.h>    /* ntfw */

#include "log.h"
#include "strl.h"
#include "util.h"
#include "cache.h"

char CAC_DIR[PATH_MAX];

const char *caches[] = {
    "sources",
    "bin",
    "logs",
};
static const int cache_len = 3;

const char *states[] = {
    "build",
    "pkg",
    "extract",
};
static const int state_len = 3;

void cache_init(void) {
    char xdg[PATH_MAX];
    pid_t pid = getpid();
    int ret;

    xdg_cache_dir(xdg, PATH_MAX);
    ret = snprintf(CAC_DIR, PATH_MAX, "%s/%ul", xdg, pid);

    if (ret == -1) {
        die("Failed to construct cache directory");
    }

    if (ret > PATH_MAX) {
        die("Cache directory exceeds PATH_MAX");
    }

    mkdir_p(CAC_DIR);

    if (chdir(CAC_DIR) != 0) {
        die("Cache directory is not accessible");
    }

    for (ret = 0; ret < state_len; ret++) {
        mkdir_p(states[ret]);
    }

    /* Drop PID portion */
    if (chdir("..") != 0) {
        die("Cache directory is not accessible");
    }

    for (ret = 0; ret < cache_len; ret++) {
        mkdir_p(caches[ret]);
    }
}

void xdg_cache_dir(char *buf, int len) {
    char *dir;
    int ret;

    dir = getenv("XDG_CACHE_HOME");

    if (!dir || !len) {
        die("Failed to construct cache directory");
    }

    if (!dir) {
        dir = getenv("HOME");

        if (!dir) {
            die("HOME is NULL");
        }

        ret = snprintf(buf, len, "%s/.cache/kiss", dir);

        if (ret == -1) {
            die("Failed to construct cache directory");
        }

        if (ret > len) {
            die("Path exceeds PATH_MAX");
        }

        return;
    }

    if (!strchr(dir, '/')) {
        die("Invalid XDG_CACHE_HOME");
    }

    ret = snprintf(buf, len, "%s/kiss", dir);

    if (ret == -1) {
        die("Failed to construct cache directory");
    }

    if (ret > len) {
        die("XDG_CACHE_HOME exceeds PATH_MAX");
    }
}

static int rm(const char *fpath, const struct stat *sb, int tf, struct FTW *fb) {
    int rv;

    // inused
    (void)(sb);
    (void)(tf);
    (void)(fb);

    rv = remove(fpath);

    if (rv) {
        msg("warning: Failed to remove %s", fpath);
    }

    return rv;
}

void cache_destroy(void) {
    nftw(CAC_DIR, rm, 64, FTW_DEPTH | FTW_PHYS);
}
