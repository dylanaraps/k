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
#include "pkg.h"
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

static const char *const actions[] = {
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

static void exit_handler(void) {
    str_free(tmp_str);

    cache_free();
    repo_free();
}

static void usage(char *arg0) {
    printf("%s [", arg0);

    for (int i = ACTION_ALT; i < ACTION_USAGE; i++) {
        printf("%c%c", actions[i + i][0], i == ACTION_USAGE - 1 ? ']' : '|');
    }

    printf(" [pkg]...\n");

    for (int i = ACTION_ALT; i < ACTION_USAGE; i++) {
        printf("%-8s %s\n", actions[i + i], actions[i + i + 1]);
    }
}

static void crux_like(void) {
    str_undo_l(&tmp_str, tmp_str->len);
    str_push_s(&tmp_str, path_normalize(getenv("PWD")));

    size_t basename = str_rchr(tmp_str, '/');

    if (!basename) {
        die("PWD is invalid");
    }

    /* vec_push(pkgs, pkg_init(tmp_str->buf + basename + 1)); */

    str_undo_l(&tmp_str, tmp_str->len - basename);
    str_push_c(&tmp_str, ':');
    str_push_s(&tmp_str, xgetenv("KISS_PATH", ""));

    if (setenv("KISS_PATH", tmp_str->buf, 1) == -1) {
        die("failed to prepend to KISS_PATH");
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
        case ACTION_BUILD:
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

            if (argc < 3) {
                crux_like();
            }
    }

    switch (action) {
        case ACTION_DOWNLOAD:
            break;

        case ACTION_LIST:
            if (argc == 2) {
                pkg_list_all(&tmp_str);

            } else {
                for (int i = 2; i < argc; i++) {
                    pkg_list_print(&tmp_str, argv[i]);
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

