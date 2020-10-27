#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <glob.h>
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

static int run_search(int argc, char *argv[], char **repos) {
    str *buf = str_init(128);

    if (!buf) {
        err("failed to allocate memory");
        return -1;
    }

    glob_t res = {0};

    for (int i = 2; i < argc; i++) {
        if (repo_glob(&res, buf, argv[i], repos) != 0 ) {
            globfree(&res);
            str_free(&buf);
            return -1;
        }

        if (res.gl_pathc == 0) {
            err("no search results for '%s'", argv[i]);
            globfree(&res);
            str_free(&buf);
            return -1;
        }

        for (size_t j = 0; j < res.gl_pathc; j++) {
            puts(res.gl_pathv[j]);
        }
        
        globfree(&res);
    }

    str_free(&buf);
    return 0;
}

static int run_action(int argc, char *argv[]) {
    struct repo *repositories = repo_create();

    if (!repositories) {
        err("failed to allocate memory");
        return -1;
    }

    if (repo_init(&repositories) != 0) {
        err("repository init failed");
        return -1;
    }

    str *cache_dir = str_init(128);

    if (!cache_dir) {
        err("failed to allocate memory");
        return -1;
    }

    if (cache_init(&cache_dir) != 0) {
        err("cache init failed"); 
        return -1;
    }

    struct pkg **pkgs = 0;

    for (int i = 2; i < argc; i++) {
        struct pkg *new = pkg_create(argv[i]);

        if (!new) {
            err("failed to allocate memory"); 
            return -1;
        }

        if (repo_find(&new->repo, argv[i], repositories->repos) != 0) {
            err("repository search error");
            return -1;
        }

        vec_push(pkgs, new);     
    }

    for (size_t i = 0; i < vec_size(pkgs); i++) {
        switch (pkg_source(pkgs[i])) {
            case -2:
                msg("[%s] no sources, skipping", pkgs[i]->name);
                break;

            case -1:
                err("[%s] failed to open sources: %s", pkgs[i]->name, 
                    strerror(errno));
                return -1;
        }
    }

    repo_free(&repositories);
    str_free(&cache_dir);
    for (size_t i = 0; i < vec_size(pkgs); i++) {
        pkg_free(&pkgs[i]);
    }
    vec_free(pkgs);

    return 0;
}

int main (int argc, char *argv[]) {
    int err = EXIT_SUCCESS;

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
        err = run_action(argc, argv);

    } else if (ARG(argv[1], "install") ||
               ARG(argv[1], "remove")) {
        //

    } else if (ARG(argv[1], "list")) {
        //

    } else if (ARG(argv[1], "search")) {
        struct repo *r = repo_create();

        if (!r) {
            err("failed to allocate memory");
            return -1;
        }

        if (repo_init(&r) != 0) {
            err("repository init failed");
            repo_free(&r);
            return -1;
        }

        err = run_search(argc, argv, r->repos);

        repo_free(&r);

    } else if (ARG(argv[1], "update")) {
        //

    } else {
        run_extension(argv + 1);
    }

    return err;
}

