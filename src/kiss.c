#include <dirent.h>
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

    puts("alt          List and swap to alternatives");
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

static void run_extension(char *argv[]) {
    char ext[64] = "kiss-";

    strncat(ext, *argv, 63);
    execvp(ext, argv);

    die("failed to execute extension %s", ext);
}

static int run_search(int argc, char *argv[], char **repos) {
    glob_t res = {0};

    for (int i = 2; i < argc; i++) {
        if (repo_glob(&res, argv[i], repos) != 0 ) {
            globfree(&res);
            return -1;
        }

        if (res.gl_pathc == 0) {
            err("no search results for '%s'", argv[i]);
            globfree(&res);
            return -1;
        }

        for (size_t j = 0; j < res.gl_pathc; j++) {
            puts(res.gl_pathv[j]);
        }
        
        globfree(&res);
    }

    return 0;
}

static int run_list(int argc, char *argv[], char *db, int fd) {
    if (argc == 2) {
        struct dirent **list = 0;

        int len = scandir(db, &list, 0, alphasort);

        if (len == -1) {
            err("failed to open repository '%s': %s", db, strerror(errno));
            return -1;
        }

        // '.' and '..'
        free(list[0]);
        free(list[1]);

        for (int i = 2; i < len; i++) {
            pkg_list(fd, list[i]->d_name);
            free(list[i]);
        }

        free(list);

    } else {
        for (int i = 2; i < argc; i++) {
            if (pkg_list(fd, argv[i]) < 0) {
                return -1;
            }
        }
    }

    return 0;
}

static int run_query(int argc, char *argv[]) {
    struct repo *r = repo_create();

    if (!r) {
        err("failed to allocate memory");
        return -1;
    }

    if (repo_init(&r, getenv("KISS_PATH")) != 0) {
        err("repository init failed");
        repo_free(&r);
        return -1;
    }

    int err = 0;

    switch (argv[1][0]) {
        case 'l':
            err = run_list(argc, argv, 
                r->list[vec_size(r->list) - 1], 
                r->fds[vec_size(r->fds) - 1]);
            break;

        case 's':
            err = run_search(argc, argv, r->list);
            break;
    }

    repo_free(&r);
    return err;
}

static int run_action(int argc, char *argv[]) {
    struct repo *repos = repo_create();

    if (!repos) {
        err("failed to allocate memory");
        return -1;
    }

    if (repo_init(&repos, getenv("KISS_PATH")) != 0) {
        err("repository init failed");
        return -1;
    }

    struct cache *cache_dir = cache_create();

    if (!cache_dir) {
        err("failed to allocate memory");
        return -1;
    }

    if (cache_init(&cache_dir) < 0) {
        err("cache init failed"); 
        return -1;
    }

    if (cache_mkdir(cache_dir) < 0) {
        err("cache creation failed"); 
        return -1; 
    }

    struct pkg **pkgs = 0;

    for (int i = 2; i < argc; i++) {
        struct pkg *new = pkg_create(argv[i]);

        if (!new) {
            err("failed to allocate memory"); 
            return -1;
        }

        new->repo = repo_find(argv[i], repos);

        if (!new->repo) {
            err("repository search error");
            return -1;
        }

        vec_push(pkgs, new);     
    }

    repo_free(&repos);
    cache_free(&cache_dir);
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

    } else if (ARG(argv[1], "list") || 
               ARG(argv[1], "search")) {
        err = run_query(argc, argv);

    } else if (ARG(argv[1], "update")) {
        //

    } else {
        run_extension(argv + 1);
    }

    return err;
}

