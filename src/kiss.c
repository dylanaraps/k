#include <dirent.h>
#include <errno.h>
#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "dir.h"
#include "log.h"
#include "vec.h"
#include "str.h"

#define DB_DIR "/var/db/kiss/installed"

typedef struct pkg {
    char *name;
    char *path;
} pkg;

static pkg  **pkgs  = NULL;
static char **repos = NULL;
static str *cac_dir = NULL;
static str *tmp_str = NULL;

enum actions {
    ACTION_ALTERNATIVES,
    ACTION_BUILD,
    ACTION_CHECKSUM,
    ACTION_DOWNLOAD,
    ACTION_EXTENSION,
    ACTION_HELPEXT,
    ACTION_INSTALL,
    ACTION_LIST,
    ACTION_REMOVE,
    ACTION_SEARCH,
    ACTION_UPDATE,
    ACTION_USAGE,
    ACTION_VERSION,
};

static const char *state_dirs[] = {
    "build",
    "extract",
    "pkg",
};

static const char *cache_dirs[] = {
    "bin",
    "logs",
    "sources",
};

// repositories {{{

static void repo_init(void) {
    str_zero(&tmp_str);
    str_push(&tmp_str, getenv("KISS_PATH"));

    char *p = NULL;

    for (char *tok = strtok_r(tmp_str->buf, ":", &p);
         tok != NULL;
         tok = strtok_r(NULL, ":", &p)) {

        if (tok[0] != '/') {
            die("relative path found in KISS_PATH");
        }

        vec_push(repos, strdup(tok));
    }

    vec_push(repos, strdup(DB_DIR));
}

static void repo_free(void) {
    for (size_t i = 0; i < vec_size(repos); i++) {
        free(repos[i]);
    }
    vec_free(repos);
}

static void repo_find_all(void) {
    for (size_t i = 0; i < vec_size(pkgs); i++) {
        glob_t buf = {0};

        for (size_t j = 0; j < vec_size(repos); j++) {
            str_zero(&tmp_str);
            str_fmt(&tmp_str, "%s/%s/", repos[j], pkgs[i]->name);

            glob(tmp_str->buf, j ? GLOB_APPEND : 0, NULL, &buf);
        }

        if (buf.gl_pathc == 0) {
            globfree(&buf);
            die("no results for '%s'", pkgs[i]->name);
        }

        for (size_t j = 0; j < buf.gl_pathc; j++) {
            puts(buf.gl_pathv[j]);
        }

        globfree(&buf);
    }
}

// }}}

// packages {{{

static pkg *pkg_init(const char *name) {
    pkg *p = malloc(sizeof(pkg));

    if (!p) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    p->name = strdup(name);

    if (!p->name || errno == ENOMEM) {
        perror("strdup");
        exit(EXIT_FAILURE);
    }

    return p;
}

static void pkg_free(void) {
    for (size_t i = 0; i < vec_size(pkgs); i++) {
        free(pkgs[i]->name);
        free(pkgs[i]);
    }
    vec_free(pkgs);
}

static int pkg_version(str **s, const char *p, const char *db) {
    str_zero(s);
    str_fmt(s, "%s/%s/version", db, p);

    FILE *f = fopen((*s)->buf, "r");

    if (!f) {
        return 1;
    }

    str_getline(s, f);
    fclose(f);

    return !(*s)->buf;
}

static void pkg_list_all(void) {
    struct dirent **list;
    int len = scandir(DB_DIR, &list, NULL, alphasort);

    // '.' and '..'
    free(list[0]);
    free(list[1]);

    str_zero(&tmp_str);

    for (int i = 2; i < len; i++) {
        if (pkg_version(&tmp_str, list[i]->d_name, DB_DIR) != 0) {
            free(list[i]);
            free(list);
            die("package '%s' not installed", pkgs[i]->name);
        }

        printf("%s %s\n", list[i]->d_name, tmp_str->buf);
        free(list[i]);
    }

    free(list);

    if (len == -1) {
        die("database not accessible");
    }
}

static void pkg_list_vec(void) {
    str_zero(&tmp_str);

    for (size_t i = 0; i < vec_size(pkgs); i++) {
        if (pkg_version(&tmp_str, pkgs[i]->name, DB_DIR) != 0) {
            die("package '%s' not installed", pkgs[i]->name);
        }

        printf("%s %s\n", pkgs[i]->name, tmp_str->buf);
    }
}

// }}}

// cache {{{

static void get_xdg_cache(str **s) {
    char *env = getenv("XDG_CACHE_HOME");

    if (env && env[0]) {
        str_fmt(s, "%s/kiss/", env);

    } else {
        env = getenv("HOME");

        if (env && env[0]) {
            str_fmt(s, "%s/.cache/kiss/", env);
        }
    }

    if (!env || !env[0]) {
        die("failed to construct cache path");
    }
}

static void cache_init(void) {
    get_xdg_cache(&cac_dir);

    if (mkdir_p(cac_dir->buf, 0755) != 0) {
        die("failed to create directory");
    }

    for (int i = 0; i < 3; i++) {
        str_push(&cac_dir, cache_dirs[i]);

        if (mkdir(cac_dir->buf, 0755) == -1 && errno != EEXIST) {
            die("failed to create directory");
        }

        str_undo(&cac_dir, cache_dirs[i]);
    }

    str_push(&cac_dir, "proc/");

    if (mkdir(cac_dir->buf, 0755) == -1 && errno != EEXIST) {
        die("failed to create directory");
    }

    pid_t pid = getpid();
    str_fmt(&cac_dir, "%u/", pid);

    if (mkdir(cac_dir->buf, 0755) == -1 && errno != EEXIST) {
        die("failed to create directory");
    }

    for (int i = 0; i < 3; i++) {
        str_push(&cac_dir, state_dirs[i]);

        if (mkdir(cac_dir->buf, 0755) == -1 && errno != EEXIST) {
            die("failed to create directory");
        }

        str_undo(&cac_dir, state_dirs[i]);
    }
}

static void cache_free(void) {
    if (cac_dir->len) {
        rm_rf(cac_dir->buf);
        str_free(&cac_dir);
    }
}

// }}}

// arguments {{{

static void exit_handler(void) {
    cache_free();
    repo_free();
    pkg_free();

    str_free(&tmp_str);
}

static void crux_like(void) {
    str_zero(&tmp_str);
    str_push(&tmp_str, getenv("PWD"));
    str_path(&tmp_str);

    char *base = strrchr(tmp_str->buf, '/');

    vec_push(pkgs, pkg_init(base + 1));
    str_undo(&tmp_str, base);
    str_push(&tmp_str, ":");
    str_push(&tmp_str, getenv("KISS_PATH"));

    int err = setenv("KISS_PATH", tmp_str->buf, 1);

    if (err == -1) {
        die("failed to prepend to KISS_PATH");
    }
}

static void run_extension(char *argv[]) {
    str_zero(&tmp_str);
    str_fmt(&tmp_str, "kiss-%s", argv[1]);

    if (execvp(tmp_str->buf, ++argv) == -1) {
        die("failed to execute extension %s", argv[0]);
    }
}

static void usage(void) {
    puts("kiss [a|b|c|d|i|l|r|s|u|v] [pkg]...");
    puts("alternatives List and swap to alternatives");
    puts("build        Build a package");
    puts("checksum     Generate checksums");
    puts("download     Pre-download all sources");
    puts("install      Install a package");
    puts("list         List installed packages");
    puts("remove       Remove a package");
    puts("search       Search for a package");
    puts("update       Update the system");
    puts("version      Package manager version");
    puts("\nRun 'kiss help-ext' to see all actions");
}

static int run_action(int action) {
    switch (action) {
        case ACTION_BUILD:
        case ACTION_CHECKSUM:
        case ACTION_DOWNLOAD:
        case ACTION_INSTALL:
        case ACTION_REMOVE: {
            cache_init();
            repo_init();

            if (vec_size(pkgs) == 0) {
                crux_like();
            }

            break;
        }
    }

    switch (action) {
        case ACTION_LIST:
            if (vec_size(pkgs)) {
                pkg_list_vec();
            } else {
                pkg_list_all();
            }
            break;

        case ACTION_SEARCH:
            repo_init();
            repo_find_all();
            break;
    }

    return 0;
}

int main (int argc, char *argv[]) {
    int action = 0;

    if (argc < 2 || !argv[1] || !argv[1][0] ||
        argv[1][0] == '-' || argc > 4096) {
        usage();
        exit(EXIT_SUCCESS);

    } else if (strcmp(argv[1], "alternatives") == 0 ||
               strcmp(argv[1], "a") == 0) {
        action = ACTION_ALTERNATIVES;

    } else if (strcmp(argv[1], "build") == 0 ||
               strcmp(argv[1], "b") == 0) {
        action = ACTION_BUILD;

    } else if (strcmp(argv[1], "checksum") == 0 ||
               strcmp(argv[1], "c") == 0) {
        action = ACTION_CHECKSUM;

    } else if (strcmp(argv[1], "download") == 0 ||
               strcmp(argv[1], "d") == 0) {
        action = ACTION_DOWNLOAD;

    } else if (strcmp(argv[1], "help-ext") == 0) {
        action = ACTION_HELPEXT;

    } else if (strcmp(argv[1], "install") == 0 ||
               strcmp(argv[1], "i") == 0) {
        action = ACTION_INSTALL;

    } else if (strcmp(argv[1], "list") == 0 ||
               strcmp(argv[1], "l") == 0) {
        action = ACTION_LIST;

    } else if (strcmp(argv[1], "remove") == 0 ||
               strcmp(argv[1], "r") == 0) {
        action = ACTION_REMOVE;

    } else if (strcmp(argv[1], "search") == 0 ||
               strcmp(argv[1], "s") == 0) {
        action = ACTION_SEARCH;

    } else if (strcmp(argv[1], "update") == 0 ||
               strcmp(argv[1], "u") == 0) {
        action = ACTION_UPDATE;

    } else if (strcmp(argv[1], "version") == 0 ||
               strcmp(argv[1], "v") == 0) {
        puts("0.0.1");
        exit(EXIT_SUCCESS);

    } else {
        run_extension(argv);
    }

    atexit(exit_handler);
    str_alloc(&tmp_str, 256);
    str_alloc(&cac_dir, 32);

    for (int i = 2; i < argc; i++) {
        vec_push(pkgs, pkg_init(argv[i]));
    }

    return run_action(action);
}

// }}}
