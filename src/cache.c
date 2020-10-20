#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include "log.h"
#include "str.h"
#include "cache.h"
#include "util.h"

static str *cache_dir = 0;

enum cache_type {
    CACHE_PERM = 0,
    CACHE_TEMP = 3,
};

static const char *cache_dirs[] = {
    "bin",   "sources", "logs",
    "build", "extract", "pkg",
};

static const size_t cache_lens[] = {
    3, 7, 4,
    5, 7, 3,
};

static void mkdir_die(str *s) {
    if (s->err != STR_OK) {
       return;
    }

    if (mkdir(s->buf, 0755) == -1) {
        switch (errno) {
            case EEXIST:
                break;

            case ENOENT:
                die("directory component does not exist '%s'", s->buf);

            default:
                die("failed to create directory '%s'", s->buf);
        }
    }
}

static void get_xdg_cache(str **s) {
    str_push_s(s, getenv("XDG_CACHE_HOME"));

    if ((*s)->err != STR_OK) {
        (*s)->err = STR_OK;
        str_push_s(s, getenv("HOME"));

        if ((*s)->err != STR_OK) {
            die("HOME is unset");
        }

        str_path_normalize(s);
        str_push_l(s, "/.cache", 7);
    }

    str_path_normalize(s);
}

static void cache_create(str *s, size_t off) {
    for (size_t i = off; i < off + 3; i++) {
        str_push_l(&s, cache_dirs[i], cache_lens[i]);
        mkdir_die(s);
        str_undo_l(&s, cache_lens[i]);
    }
}

str *cache_init(void) {
    cache_dir = str_init_die(64);

    get_xdg_cache(&cache_dir);
    mkdir_die(cache_dir);

    str_push_l(&cache_dir, "/kiss/", 6);
    mkdir_die(cache_dir);

    cache_create(cache_dir, CACHE_PERM);

    str_push_l(&cache_dir, "proc", 4);
    mkdir_die(cache_dir);

    str_printf(&cache_dir, "/%u/", getpid());
    mkdir_die(cache_dir);

    cache_create(cache_dir, CACHE_TEMP);

    if (cache_dir->err != STR_OK) {
        die("failed to create cache");
    }

    return cache_dir;
}

void cache_free(void) {
    if (cache_dir && cache_dir->err == STR_OK) {
        rm_rf(cache_dir->buf);
    }

    str_free(cache_dir);
}
