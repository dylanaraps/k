#include <dirent.h>
#include <errno.h>
#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "util.h"
#include "vec.h"
#include "str.h"

#define DB_DIR "/var/db/kiss/installed"

typedef struct pkg {
    char *name;
    char *path;
} pkg;

static pkg  *pkgs   = NULL;
static char **repos = NULL;

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
    char *p = NULL;

    str path = {0};
    str_cat(&path, getenv("KISS_PATH"));

    for (char *tok = strtok_r(path.buf, ":", &p);
         tok != NULL;
         tok = strtok_r(NULL, ":", &p)) {

        if (tok[0] != '/') {
            die("relative path found in KISS_PATH");
        }

        vec_add(repos, strdup(tok));
    }

    free(p);
    str_free(&path);

    vec_add(repos, strdup(DB_DIR));
}

static void repo_free(void) {
    for (size_t i = 0; i < vec_size(repos); i++) {
        free(repos[i]);
    }
    vec_free(repos);
}

static glob_t repo_glob(const char *pattern) {
    glob_t buf = {0};

    if (vec_size(repos) == 0) {
        repo_init();
    }

    for (size_t i = 0; i < vec_size(repos); ++i) {
        str query = {0};

        str_cat(&query, repos[i]);
        str_cat(&query, "/");
        str_cat(&query, pattern);
        str_cat(&query, "/");

        glob(query.buf, i ? GLOB_APPEND : 0, NULL, &buf);

        str_free(&query);
    }

    return buf;
}

static void repo_find_all(void) {
    for (size_t i = 0; i < vec_size(pkgs); ++i) {
        glob_t buf = repo_glob(pkgs[i].name);

        if (buf.gl_pathc == 0) {
            globfree(&buf);
            die("no results for '%s'", pkgs[i].name);
        }

        for (size_t j = 0; j < buf.gl_pathc; j++) {
            puts(buf.gl_pathv[j]);
        }

        globfree(&buf);
    }
}

// }}}

// packages {{{

static pkg pkg_init(const char *name) {
    pkg p = {0};

    p.name = strdup(name);

    return p;
}

static void pkg_free(void) {
    for (size_t i = 0; i < vec_size(pkgs); i++) {
        free(pkgs[i].name);
        free(pkgs[i].path);
    }
    vec_free(pkgs);
}

static char *pkg_version(const char *name, const char *path) {
    str file = {0};

    str_cat(&file, path);
    str_cat(&file, "/");
    str_cat(&file, name);
    str_cat(&file, "/version");

    FILE *f = fopen(file.buf, "r");

    str_free(&file);

    if (!f) {
        return NULL;
    }

    char *ver = NULL;
    int err = getline(&ver, &(size_t){0}, f);
    fclose(f);

    if (err == -1) {
        return NULL;
    }

    char *tmp = strchr(ver, '\n');

    if (tmp) {
        *tmp = 0;
    }

    return ver;
}

static void pkg_list_all(void) {
    if (vec_size(pkgs) == 0) {
        struct dirent **list;
        int len = scandir(DB_DIR, &list, NULL, alphasort);

        // '.' and '..'
        free(list[0]);
        free(list[1]);

        for (int i = 2; i < len; i++) {
            vec_add(pkgs, pkg_init(list[i]->d_name));
            free(list[i]);
        }
        free(list);

        if (len == -1) {
            die("database not accessible");
        }
    }

    for (size_t i = 0; i < vec_size(pkgs); ++i) {
        char *ver = pkg_version(pkgs[i].name, DB_DIR);

        if (!ver) {
            die("package '%s' not installed", pkgs[i].name);
        }

        printf("%s %s\n", pkgs[i].name, ver);
        free(ver);
    }
}

// }}}

// cache {{{

static void get_xdg_cache(str *s) {
    str_cat(s, getenv("XDG_CACHE_HOME"));

    if (s->buf[0]) {
        str_cat(s, "/kiss");

    } else {
        str_cat(s, getenv("HOME"));

        if (s->buf[0]) {
            str_cat(s, "/.cache/kiss");
        }
    }

    if (!s->buf) {
        die("failed to construct cache path");
    }
}

static void cache_init(str *cac) {
    get_xdg_cache(cac);

    if (mkdir_p(cac->buf, 0755) == 0) {
        for (int i = 0; i < 3; i++) {
            str tmp = {0};
            str_cat(&tmp, cac->buf);
            str_cat(&tmp, "/");
            str_cat(&tmp, cache_dirs[i]);

            if (mkdir(tmp.buf, 0755) == -1 && errno != EEXIST) {
                str_free(&tmp);
                die("failed to create directory %s", tmp.buf);
            }

            str_free(&tmp);
        }

        char *pid = pid_to_str(getpid());

        str_cat(cac, "/");
        str_cat(cac, pid);

        if (mkdir_e(cac->buf, 0755) != 0) {
            free(pid);
            die("failed to create directory %s", cac->buf);
        }

        for (int i = 0; i < 3; i++) {
            str tmp = {0};
            str_cat(&tmp, cac->buf);
            str_cat(&tmp, "/");
            str_cat(&tmp, state_dirs[i]);

            if (mkdir(tmp.buf, 0755) == -1 && errno != EEXIST) {
                str_free(&tmp);
                free(pid);
                die("failed to create directory %s", tmp.buf);
            }

            str_free(&tmp);
        }

        free(pid);

    } else {
        die("failed to create directory %s", cac->buf);
    }
}

// }}}

// arguments {{{

static void crux_like(void) {
    str cwd = {0};
    str_cat(&cwd, getenv("PWD"));

    if (!cwd.buf || !cwd.buf[0]) {
        die("PWD is unset");
    }

    vec_add(pkgs, pkg_init(path_basename(cwd.buf)));

    int err = PATH_prepend(cwd.buf, "KISS_PATH");
    str_free(&cwd);

    if (err == 1) {
        die("failed to prepend to KISS_PATH");
    }
}

static void run_extension(char *argv[]) {
    str cmd = {0};
    str_cat(&cmd, "kiss-");
    str_cat(&cmd, argv[1]);

    int err = execvp(cmd.buf, ++argv);

    str_free(&cmd);

    if (err == -1) {
        die("failed to execute extension %s", argv[0]);
    }
}

static int run_action(int action, char **argv, int argc) {
    atexit(repo_free);
    atexit(pkg_free);

    for (int i = 2; i < argc; i++) {
        vec_add(pkgs, pkg_init(argv[i]));
    }

    switch (action) {
        case ACTION_BUILD:
        case ACTION_CHECKSUM:
        case ACTION_DOWNLOAD:
        case ACTION_INSTALL:
        case ACTION_REMOVE: {
            str cac = {0};
            cache_init(&cac);
            str_free(&cac);

            if (vec_size(pkgs) == 0) {
                crux_like();
            }

            break;
        }
    }

    switch (action) {
        case ACTION_LIST:
            pkg_list_all();
            break;

        case ACTION_SEARCH:
            repo_find_all();
            break;

        case ACTION_EXTENSION:
            run_extension(argv);
            break;

        case ACTION_VERSION:
            puts("0.0.1");
            break;

        default:
            puts("kiss [b|c|d|l|s|v] [pkg]...");
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

    return 0;
}

int main (int argc, char *argv[]) {
    int action = 0;

    if (argc < 2 || !argv[1] || !argv[1][0] || argv[1][0] == '-') {
        action = ACTION_USAGE;

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
        action = ACTION_VERSION;

    } else {
        action = ACTION_EXTENSION;
    }

    return run_action(action, argv, argc);
}

// }}}
