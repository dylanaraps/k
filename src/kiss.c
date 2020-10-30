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

static int run_extension(char *argv[]) {
    char ext[64] = "kiss-";

    strncat(ext, *argv, 63);
    execvp(ext, argv);

    err("failed to execute extension %s", ext);
    return -1;
}

static int run_download(struct pkg **p) {
    for (size_t i = 0; i < vec_size(p); i++) {
        FILE *src_file = pkg_fopen(p[i]->repo, p[i]->name, "sources");

        if (!src_file) {
            err("failed to open sources file '%s': %s", 
                p[i]->name, strerror(errno));
            return -1;
        }

        char *line  = 0;
        ssize_t len = 0;
        size_t size = 0;

        while ((len = getline(&line, &size, src_file)) > 0) {
            if (line[0] == '\n' || line[0] == '#') {
                continue; 
            }

            if (line[len - 1] == '\n') {
                line[len - 1] = 0;
            }

            printf("%s\n", line);
        }

        free(line);
        fclose(src_file);
    }

    return 0;
}

static int run_search(int argc, char *argv[], struct repo *r) {
    for (int i = 2; i < argc; i++) {
        glob_t res;

        if (!argv[i][0]) {
            continue;
        }

        if (repo_glob(&res, argv[i], r) != 0 ) {
            goto error;
        }

        if (res.gl_pathc == 0) {
            err("no search results for '%s'", argv[i]);
            goto error;
        }

        for (size_t j = 0; j < res.gl_pathc; j++) {
            puts(res.gl_pathv[j]);
        }

        globfree(&res);
        continue;
error:
        globfree(&res);
        return -1;
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

    int err = 0;

    switch (argv[1][0]) {
        case 'l':
            if ((err = repo_get_db(&r->mem)) < 0) {
                err("database init failed");
                goto error;
            }

            if ((err = repo_add(&r, r->mem)) < 0) {
                goto error;
            }

            err = run_list(argc, argv, r->list[0], r->fds[0]);
            break;

        case 's':
            if ((err = repo_init(&r, getenv("KISS_PATH"))) < 0) {
                err("repository init failed");
                goto error;
            }

            err = run_search(argc, argv, r);
            break;
    }

error:
    repo_free(&r);
    return err;
}

static int run_action(int argc, char *argv[]) {
    struct repo *repos = repo_create();

    if (!repos) {
        err("failed to allocate memory");
        return -1;
    }
    
    int err = 0;

    if (repo_init(&repos, getenv("KISS_PATH")) < 0) {
        err("repository init failed");
        err = -1;
        goto free_repo;
    }

    struct cache *cache_dir = cache_create();

    if (!cache_dir) {
        err("failed to allocate memory");
        err = -1;
        goto free_cache;
    }

    if (cache_init(&cache_dir) < 0) {
        err("cache init failed"); 
        err = -1;
        goto free_cache;
    }

    struct pkg **pkgs = 0;

    for (int i = 2; i < argc; i++) {
        struct pkg *new = pkg_create(argv[i]);

        if (!new) {
            err("failed to allocate memory"); 
            err = -1;
            goto free_pkg;
        }

        new->repo = repo_find(argv[i], repos);

        if (new->repo < 0) {
            err("repository search error");
            err = -1;
            goto free_pkg;
        }

        vec_push(pkgs, new);     
    }

    switch (argv[1][0]) {
        case 'b':
            break;

        case 'c':
            break;

        case 'd':
            run_download(pkgs);
            break;
    }

free_pkg:
    for (size_t i = 0; i < vec_size(pkgs); i++) {
        pkg_free(&pkgs[i]);
    }
    vec_free(pkgs);

free_cache:
    cache_free(&cache_dir);

free_repo:
    repo_free(&repos);

    return err;
}

int main (int argc, char *argv[]) {
    int err = 0;

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
        err = run_extension(argv + 1);
    }

    return err;
}

