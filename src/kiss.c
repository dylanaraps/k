#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <sys/wait.h>

#include "cache.h"
#include "download.h"
#include "file.h"
#include "repo.h"
#include "pkg.h"
#include "str.h"
#include "util.h"
#include "vec.h"

static void usage(char *arg0) {
    fputs(arg0, stdout);
    fputs(" [a|b|c|d|i|l|r|s|u|v] [pkg]...\n", stdout);

    puts("build        Build a package");
    puts("checksum     Generate checksums");
    puts("download     Pre-download all sources");
    puts("install      Install a package");
    puts("remove       Remove a package");
    puts("update       Update the system");
    puts("version      Package manager version");
}

static void run_extension(char *argv[]) {
    char ext[64] = "kiss-";

    strncat(ext, *argv, 63);
    execvp(ext, argv);

    die("failed to execute extension %s", ext);
}

static int run_action(int argc, char *argv[]) {
    struct repo *repositories = repo_create();

    if (!repositories) {
        die("failed to allocate memory");
    }

    if (repo_init(&repositories) != 0) {
        die("repository init failed");
    }

    str *cache_dir = str_init(128);

    if (!cache_dir) {
        die("failed to allocate memory");
    }

    if (cache_init(&cache_dir) != 0) {
        die("cache init failed"); 
    }

    struct pkg **pkgs = 0;

    for (int i = 2; i < argc; i++) {
        struct pkg *new = pkg_create(argv[i]);

        if (!new) {
            die("failed to allocate memory"); 
        }

        if (repo_find(&new->repo, argv[i], repositories->repos) != 0) {
            die("repository search error");
        }

        vec_push(pkgs, new);     
    }

    for (size_t i = 0; i < vec_size(pkgs); i++) {
        switch (pkg_source(pkgs[i])) {
            case -2:
                msg("[%s] no sources, skipping", pkgs[i]->name);
                break;

            case -1:
                die("[%s] failed to open sources: %s", pkgs[i]->name, 
                    strerror(errno));
        }
    }

    repo_free(&repositories);
    str_free(&cache_dir);
    for (size_t i = 0; i < vec_size(pkgs); i++) {
        pkg_free(&pkgs[i]);
    }
    vec_free(pkgs);
    return EXIT_SUCCESS;
}

int main (int argc, char *argv[]) {
    if (argc < 2 || !argv[1] || !argv[1][0] || argv[1][0] == '-') {
        usage(argv[0]);

// Check if argument matches an action. True for b==build and build==build
// strcmp is only reached when both first characters match.
#define ARG(a, b) ((a[0]) == (b[0]) && ((!a[1]) || strcmp(a, b) == 0)) 

    } else if (ARG(argv[1], "version")) {
        puts("0.0.1");

    } else if (ARG(argv[1], "alt")) {
        //

    } else if (ARG(argv[1], "build") ||
               ARG(argv[1], "checksum") ||
               ARG(argv[1], "download")) {
        run_action(argc, argv);

    } else if (ARG(argv[1], "install") ||
               ARG(argv[1], "remove")) {
        //

    } else if (ARG(argv[1], "list")) {
        //

    } else if (ARG(argv[1], "search")) {
        //

    } else if (ARG(argv[1], "update")) {
        //

    } else {
        run_extension(argv + 1);
    }

    return EXIT_SUCCESS;
}

