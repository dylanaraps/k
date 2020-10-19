#include <errno.h>
#include <dirent.h>
#include <glob.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "cache.h"
#include "log.h"
#include "str.h"
#include "vec.h"
#include "repo.h"
#include "util.h"

// Check equality of arguments. If both first characters match, the comparison
// continues, if not the strcmp() call is skipped entirely. This matches
// 'b' -> 'b' or 'build' -> 'build'.
#define ARG(a, n) ((a[0]) == (n[0]) && ((!a[1]) || strcmp(a, n) == 0))

// String for use with throwaway string operations. A chunk of memory is
// allocated early on and is used in place of multiple temporary variables. This
// removes the need to keep allocating memory. Functions using this should first
// run str_undo_l(&tmp_str, tmp_str->len); to reset the string.
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

    // hidden actions
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

static void pkg_version(str **s, const char *name, const char *repo) {
    str_undo_l(s, tmp_str->len);
    str_push_s(s, repo);
    str_push_c(s, '/');
    str_push_s(s, name);
    str_push_l(s, "/version", 8);

    if ((*s)->err == STR_OK) {
        FILE *f = fopen((*s)->buf, "r");

        if (f) {
            str_undo_l(s, (*s)->len);
            str_getline(s, f);
            fclose(f);

            if ((*s)->err == STR_OK && (*s)->len > 0) {
                return;
            }
        }
    }

    (*s)->err = STR_ERROR;
}

static void pkg_list_print(char *name) {
    pkg_version(&tmp_str, name, get_db_dir());

    if (tmp_str->err == STR_OK) {
        printf("%s %s\n", name, tmp_str->buf);

    } else {
        die("package '%s' not installed", name);
    }
}

static void pkg_list_all(void) {
    struct dirent **list;

    int len = scandir(get_db_dir(), &list, 0, alphasort);

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

static void exit_handler(void) {
    str_free(tmp_str);

    cache_free();
    repo_free();
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
    // Actions requiring temporary buffer.
    switch (action) {
        case ACTION_DOWNLOAD:
        case ACTION_EXT:
        case ACTION_LIST:
        case ACTION_SEARCH:
            if (!(tmp_str = str_init(256))) {
                die("failed to allocate memory");
            }
    }

    // Actions requiring cache access.
    switch (action) {
        case ACTION_BUILD:
        case ACTION_CHECKSUM:
        case ACTION_DOWNLOAD:
        case ACTION_INSTALL:
        case ACTION_REMOVE:
            cache_init();
    }

    // Actions requiring repository access.
    switch (action) {
        case ACTION_BUILD:
        case ACTION_CHECKSUM:
        case ACTION_DOWNLOAD:
        case ACTION_INSTALL:
        case ACTION_LIST:
        case ACTION_REMOVE:
        case ACTION_SEARCH:
            repo_init();
    }

    // Actions that take package lists as input.
    switch (action) {
        case ACTION_BUILD:
        case ACTION_CHECKSUM:
        case ACTION_DOWNLOAD:
        case ACTION_INSTALL:
        case ACTION_REMOVE:
            for (int i = 2; i < argc; i++) {
                //
            }
    }

    switch (action) {
        case ACTION_DOWNLOAD:
            break;

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
                repo_find_all(&tmp_str, argv[i]);
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
            if (ARG(argv[1], actions[i + i])) {
                action = i;
                break;
            }
        }

        atexit(exit_handler);
    }

    return run_action(action, argc, argv);
}

