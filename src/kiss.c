#include <dirent.h>
#include <glob.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "log.h"
#include "str.h"
#include "vec.h"
#include "util.h"

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

enum kiss_actions {
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

    // '.' and '..'
    free(list[0]);
    free(list[1]);

    for (int i = 2; i < len; i++) {
        pkg_list_print(list[i]->d_name);
        free(list[i]);
    }

    free(list);

    if (len == -1) {
        die("database not accessible");
    }
}

// }}}

// Arguments {{{

static void exit_handler(void) {
    str_free(tmp_str);
    str_free(KISS_PATH);
    vec_free(repos);
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

static int run_action(enum kiss_actions action, int argc, char *argv[]) {
    switch (action) { // throwaway buffer.
        case ACTION_EXTENSION:
        case ACTION_LIST:
        case ACTION_SEARCH:
            if (!(tmp_str = str_init(256))) {
                die("failed to allocate memory");
            }

        // to make compiler happy, temporary
        default:
            break;
    }

    switch (action) { // actions requiring repository access.
        case ACTION_LIST:
        case ACTION_SEARCH:
            repo_init();

        // to make compiler happy, temporary
        default:
            break;
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

        case ACTION_EXTENSION:
            run_extension(argv + 1);
            break;

        default:
            usage();
            break;
    }

    return EXIT_SUCCESS;
}

int main (int argc, char *argv[]) {
    enum kiss_actions action = ACTION_USAGE;

    if (argc < 2 || !argv[1] || !argv[1][0] ||
        argv[1][0] == '=' || argc > 1024) {
        action = ACTION_USAGE;

    } else if ((argv[1][0] == 'a') && (!argv[1][1] ||
               strcmp(argv[1], "alternatives") == 0)) {
        action = ACTION_ALTERNATIVES;

    } else if ((argv[1][0] == 'b') && (!argv[1][1] ||
               strcmp(argv[1], "build") == 0)) {
        action = ACTION_BUILD;

    } else if ((argv[1][0] == 'c') && (!argv[1][1] ||
               strcmp(argv[1], "checksum") == 0)) {
        action = ACTION_CHECKSUM;

    } else if ((argv[1][0] == 'd') && (!argv[1][1] ||
               strcmp(argv[1], "download") == 0)) {
        action = ACTION_DOWNLOAD;

    } else if ((argv[1][0] == 'i') && (!argv[1][1] ||
               strcmp(argv[1], "install") == 0)) {
        action = ACTION_INSTALL;

    } else if ((argv[1][0] == 'l') && (!argv[1][1] ||
               strcmp(argv[1], "list") == 0)) {
        action = ACTION_LIST;

    } else if ((argv[1][0] == 'r') && (!argv[1][1] ||
               strcmp(argv[1], "remove") == 0)) {
        action = ACTION_REMOVE;

    } else if ((argv[1][0] == 's') && (!argv[1][1] ||
               strcmp(argv[1], "search") == 0)) {
        action = ACTION_SEARCH;

    } else if ((argv[1][0] == 'u') && (!argv[1][1] ||
               strcmp(argv[1], "update") == 0)) {
        action = ACTION_UPDATE;

    } else if ((argv[1][0] == 'v') && (!argv[1][1] ||
               strcmp(argv[1], "version") == 0)) {
        action = ACTION_VERSION;

    } else {
        action = ACTION_EXTENSION;
    }

    atexit(exit_handler);

    return run_action(action, argc, argv);
}

// }}}
