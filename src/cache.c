#define _POSIX_C_SOURCE 200809L
#include <stdlib.h> /* size_t */
#include <stdio.h>  /* snprintf */
#include <unistd.h> /* getenv */
#include <limits.h> /* PATH_MAX */
#include <string.h> /* strchr */
#include <libgen.h> /* dirname */
#include <errno.h>  /* errno */
#include <fcntl.h>  /* O_CREAT */
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


static void xdg_cache_dir(char *buf, int len) {
    char *dir;

    if (!len) {
        die("Failed to construct cache directory");
    }

    dir = getenv("XDG_CACHE_HOME");

    if (!dir || !dir[0]) {
        dir = getenv("HOME");

        if (!dir || !dir[0]) {
            die("HOME is NULL");
        }

        xsnprintf(buf, len, "%s/.cache/kiss", dir);

        return;
    }

    if (!strchr(dir, '/')) {
        die("XDG_CACHE_HOME and HOME must be absolute");
    }

    xsnprintf(buf, len, "%s/kiss", dir);
}

void cache_init(void) {
    char xdg[PATH_MAX];
    char tmp[PATH_MAX];
    pid_t pid = getpid();
    int i;

    xdg_cache_dir(xdg, PATH_MAX);
    xsnprintf(CAC_DIR, PATH_MAX, "%s/%u", xdg, pid);
    mkdir_p(CAC_DIR);

    for (i = 0; i < state_len; i++) {
        xsnprintf(tmp, PATH_MAX, "%s/%s", CAC_DIR, states[i]);
        mkdir_p(tmp);
    }

    for (i = 0; i < cache_len; i++) {
        xsnprintf(tmp, PATH_MAX, "%s/%s", xdg, caches[i]);
        mkdir_p(tmp);
    }
}

void state_init(package *pkg, const char *type, char *buf) {
    xsnprintf(buf, PATH_MAX, "%s/%s/%s", CAC_DIR, type, pkg->name);

    if (!buf) {
        die("[%s] Failed to init cache directory %s", type, pkg->name);
    }

    mkdir_p(buf);
}

void cache_destroy(void) {
    rm_dir(CAC_DIR);
}

