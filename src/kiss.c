#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

static pkg **pkgs = 0;

static void exit_handler(void) {
    str_free(&tmp_str);
    cache_free();
    repo_free();

    for (size_t i = 0; i < vec_size(pkgs); i++) {
        pkg_free(&pkgs[i]);
    }

    vec_free(pkgs);
}

static void usage(char *arg0) {
    fputs(arg0, stdout);
    fputs(" [a|b|c|d|i|l|r|s|u|v] [pkg]...\n", stdout);

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
}

static void crux_like(str **s) {
    str_undo_l(s, (*s)->len);
    str_push_s(s, getenv("PWD"));
    str_path_normalize(s);

    size_t basename = str_rchr(*s, '/');

    if (!basename) {
        die("PWD is invalid");
    }

    if ((*s)->err == STR_OK) {
        vec_push(pkgs, pkg_init(tmp_str->buf + basename + 1));
    }

    str_undo_l(s, (*s)->len - basename);
    str_push_c(s, ':');
    str_push_s(s, xgetenv("KISS_PATH", ""));

    if ((*s)->err != STR_OK) {
        die("failed to construct string");
    }

    if (setenv("KISS_PATH", (*s)->buf, 1) == -1) {
        die("failed to prepend to KISS_PATH");
    }
}

static void run_extension(char *argv[]) {
    str *ext = str_init_die(32);

    str_push_l(&ext, "kiss-", 5);
    str_push_s(&ext, *argv);

    if (ext->err != STR_OK) {
        die("failed to construct string 'kiss-%s'", *argv);
    }

    execvp(ext->buf, argv);
    die("failed to execute extension kiss-%s", *argv);
}

static int run_query(int argc, char *argv[]) {
    tmp_str = str_init_die(64);
    char **repo = repo_init();

    switch (argv[1][0]) {
        case 'l':
            if (argc == 2) {
                pkg_list_installed(&tmp_str, repo[vec_size(repo) - 1]);

            } else {
                for (int i = 2; i < argc; i++) {
                    pkg_list_print(&tmp_str, argv[i], repo[vec_size(repo) - 1]);
                }
            }
            break;

        case 's':
            if (argc == 2) {
                msg("usage: %s %s <search query>", argv[0], argv[1]);

            } else {
                for (int i = 2; i < argc; i++) {
                    repo_find_all(&tmp_str, argv[i]);
                }
            }
            break;
    }

    return EXIT_SUCCESS;
}

static int run_action(int argc, char *argv[]) {
    tmp_str = str_init_die(512);

    repo_init();
    cache_init();

    if (argc < 3) {
        crux_like(&tmp_str);

    } else {
        for (int i = 2; i < argc; i++) {
            if (strchr(argv[i], '/')) {
                die("Argument contains invalid char '/'");
            }

            vec_push(pkgs, pkg_init(argv[i]));
        }
    }

    switch (argv[1][0]) {
        case 'd':
            break;
    }

    return EXIT_SUCCESS;
}

int main (int argc, char *argv[]) {
    atexit(exit_handler);

    if (argc < 2 || !argv[1] || !argv[1][0] ||
        argv[1][0] == '-' || argc > 1024) {
        usage(argv[0]);

    } else if (ARG(argv[1], "version")) {
        puts("0.0.1");

    } else if (ARG(argv[1], "list") ||
               ARG(argv[1], "search")) {
        run_query(argc, argv);

    } else if (ARG(argv[1], "alt")      ||
               ARG(argv[1], "build")    ||
               ARG(argv[1], "checksum") ||
               ARG(argv[1], "download") ||
               ARG(argv[1], "install")  ||
               ARG(argv[1], "remove")   ||
               ARG(argv[1], "update")) {
        run_action(argc, argv);

    } else {
        run_extension(argv + 1);
    }

    return EXIT_SUCCESS;
}

