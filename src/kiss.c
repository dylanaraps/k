#include <dirent.h>
#include <glob.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "log.h"
#include "str.h"
#include "vec.h"
#include "util.h"

#define ARG(a, n) ((a[0]) == (n[0]) && ((!a[1]) || strcmp(a, n) == 0))

// String holding the value of getenv("KISS_PATH") in addition to
// getenv("KISS_ROOT")/var/db/kiss/installed.
static str *KISS_PATH = 0;

// Vector holding pointers to each individual repository, fields split from
// KISS_PATH. ie, they share the same memory.
static char **repos = 0;

// String for use with throwaway string operations. A chunk of memory is
// allocated early on and is used in place of multiple temporary variables. This
// removes the need to keep allocating memory. Functions using this should first
// run str_undo_l(&tmp_str, tmp_str->len); to reset the string. This will not
// zero the memory, use str_zero(&tmp_str); if needed. This memory is later
// freed at exit.
static str *tmp_str = 0;

enum action_type {
    ACTION_ALT,
    ACTION_BUILD,
    ACTION_CHECKSUM,
    ACTION_DOWNLOAD,
    ACTION_INSTALL,
    ACTION_LIST,
    ACTION_REMOVE,
    ACTION_SEARCH,
    ACTION_UPDATE,
    ACTION_VERSION,

    ACTION_USAGE,
    ACTION_EXT,
    ACTION_HELPEXT,
};

static const char *actions[] = {
    "alt",      "list and swap to alternatives",
    "build",    "build packages",
    "checksum", "generate checksums",
    "download", "pre-download sources",
    "install",  "install packages",
    "list",     "list installed packages",
    "remove",   "remove packages",
    "search",   "search for packages (glob)",
    "update",   "update the system",
    "version",  "show version information",
};

// Repositories {{{

static void repo_init(void) {
    if (!(KISS_PATH = str_init(128))) {
        die("failed to allocate memory");
    }

    str_push_s(&KISS_PATH, xgetenv("KISS_PATH", ""));
    str_push_c(&KISS_PATH, ':');
    str_push_s(&KISS_PATH, xgetenv("KISS_ROOT", "/"));
    str_push_l(&KISS_PATH, "var/db/kiss/installed", 21);

    if (KISS_PATH->err != STR_OK) {
        die("failed to read KISS_PATH");
    }

    for (char *t = strtok(KISS_PATH->buf, ":"); t; t = strtok(0, ":")) {
        if (t[0] != '/') {
            die("relative path found in KISS_PATH");
        }

        vec_push(repos, path_normalize(t));
    }
}

static void repo_find_all(char *query) {
    glob_t buf = {0};

    for (size_t j = 0; j < vec_size(repos); j++) {
        str_undo_l(&tmp_str, tmp_str->len);
        str_push_s(&tmp_str, repos[j]);
        str_push_c(&tmp_str, '/');
        str_push_s(&tmp_str, query);
        str_push_c(&tmp_str, '/');

        if (tmp_str->err != STR_OK) {
            die("failed to construct glob");
        }

        glob(tmp_str->buf, j ? GLOB_APPEND : 0, 0, &buf);
    }

    if (buf.gl_pathc == 0) {
        globfree(&buf);
        die("no results for '%s'", query);
    }

    for (size_t j = 0; j < buf.gl_pathc; j++) {
        puts(buf.gl_pathv[j]);
    }

    globfree(&buf);
}

// }}}

// Packages {{{

static char *pkg_version(char *pkg, char *repo) {
    str_undo_l(&tmp_str, tmp_str->len);
    str_push_s(&tmp_str, repo);
    str_push_c(&tmp_str, '/');
    str_push_s(&tmp_str, pkg);
    str_push_l(&tmp_str, "/version", 8);

    if (tmp_str->err != STR_OK) {
        return NULL;
    }

    FILE *f = fopen(tmp_str->buf, "r");

    if (!f) {
        return NULL;
    }

    str_getline(&tmp_str, f);
    fclose(f);

    return tmp_str->err == STR_OK ? tmp_str->buf : 0;
}

static void pkg_list_print(char *pkg) {
    char *ver = pkg_version(pkg, repos[vec_size(repos) - 1]);

    if (ver) {
        printf("%s %s\n", pkg, ver);

    } else {
        die("package '%s' not installed", pkg);
    }
}

static void pkg_list_all(void) {
    struct dirent **list;

    int len = scandir(repos[vec_size(repos) - 1], &list, 0, alphasort);

    if (len == -1) {
        die("database not accessible");
    }

    // '.' and '..'
    free(list[0]);
    free(list[1]);

    for (int i = 2; i < len; i++) {
        pkg_list_print(list[i]->d_name);
        free(list[i]);
    }

    free(list);
}

// }}}

// Arguments {{{

static void exit_handler(void) {
    str_free(tmp_str);
    str_free(KISS_PATH);
    vec_free(repos);
}

static void usage(char *arg0) {
    printf("%s [", arg0);

    for (int i = ACTION_ALT; i < ACTION_USAGE * 2; i += 2) {
        printf("%c%c", actions[i][0], i == ACTION_VERSION ? ']' : '|');
    }

    printf(" [pkg]...\n");

    for (int i = ACTION_ALT; i < ACTION_USAGE * 2; i += 2) {
        printf("%-8s %s\n", actions[i], actions[i + 1]);
    }
}

static void run_extension(char *argv[]) {
    str_undo_l(&tmp_str, tmp_str->len);
    str_push_l(&tmp_str, "kiss-", 5);
    str_push_s(&tmp_str, *argv);

    if (tmp_str->err != STR_OK) {
        die("failed to construct string 'kiss-%s'", *argv);
    }

    execvp(tmp_str->buf, argv);
    die("failed to execute extension %s", *argv);
}

static int run_action(int action, int argc, char *argv[]) {
    switch (action) {
        case ACTION_EXT:
        case ACTION_LIST:
        case ACTION_SEARCH:
            if (!(tmp_str = str_init(256))) {
                die("failed to allocate memory");
            }
    }

    switch (action) {
        case ACTION_LIST:
        case ACTION_SEARCH:
            repo_init();
    }

    switch (action) {
        case ACTION_LIST:
            if (argc == 2) {
                pkg_list_all();

            } else {
                for (int i = 2; i < argc; i++) {
                    pkg_list_print(argv[i]);
                }
            }
            break;

        case ACTION_SEARCH:
            for (int i = 2; i < argc; i++) {
                repo_find_all(argv[i]);
            }
            break;

        case ACTION_VERSION:
            puts("0.0.1");
            break;

        case ACTION_EXT:
            run_extension(argv + 1);
            break;

        default:
            usage(argv[0]);
            break;
    }

    return EXIT_SUCCESS;
}

int main (int argc, char *argv[]) {
    int action = ACTION_EXT;

    if (argc < 2 || !argv[1] || !argv[1][0] ||
        argv[1][0] == '-' || argc > 1024) {
        action = ACTION_USAGE;

    } else {
        for (int i = ACTION_ALT; i < ACTION_USAGE; i++) {
            action = ARG(argv[1], actions[i + i]) ? i : action;
        }

        atexit(exit_handler);
    }

    return run_action(action, argc, argv);
}

// }}}
