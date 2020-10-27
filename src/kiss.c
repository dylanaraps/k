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
#include "str.h"
#include "util.h"
#include "vec.h"

struct pkg {
    char *name;
    char *repo;
};

// pkgs {{{

static void pkg_free(struct pkg **p) {
    free((*p)->name); 
    free(*p);
    *p = NULL;
}

static FILE *pkg_fopen(struct pkg *p, const char *f) {
    int repo_fd = open(p->repo, O_RDONLY);    

    if (repo_fd == -1) {
        return NULL;
    }

    int pkg_fd = openat(repo_fd, p->name, O_RDONLY);
    close(repo_fd);

    if (pkg_fd == -1) {
        return NULL;
    }

    int fd = openat(pkg_fd, f, O_RDONLY);
    close(pkg_fd);

    if (fd == -1) {
        return NULL;
    }

    return fdopen(fd, "r");
}

static int pkg_source(struct pkg *p) {
    FILE *f = pkg_fopen(p, "sources");

    if (!f) {
        return errno == ENOENT ? -3 : -1;
    }

    str *tmp = str_init(256);

    if (!tmp) {
        return -3;
    }

    char *line = 0;

    while (getline(&line, &(size_t){0}, f) != -1) {
        if (line[0] == '#' || line[0] == '\n') {
            continue; 
        }

        char *src = strtok(line, " \n");
        char *des = strtok(NULL, " \n");

        if (!src) {
            return -2;
        }

        if (strncmp(src, "git+", 4) == 0) {
            msg("[%s] found git %s", p->name, src);    

        } else if (strstr(src, "://")) {

        }
    }

    str_free(&tmp);
    free(line);
    fclose(f);

    return 0;
}

static struct pkg *pkg_init(const char *name) {
    struct pkg *p = malloc(sizeof *p);    

    if (!p) {
        die("failed to allocate memory");
    }

    if (!name || !name[0]) {
        die("invalid input");
    }

    p->name = strdup(name);

    if (!p->name) {
        die("failed to allocate memory");
    }

    switch (repo_find(&p->repo, name)) {
        case -1:
            die("error during search: %s", strerror(errno));

        case -2:
            die("package '%s' not in any repository", name);

        case -3:
            die("repo string error");
    }

    return p;
}

static void pkg_free_all(struct pkg **p) {
    for (size_t i = 0; i < vec_size(p); i++) {
        pkg_free(&p[i]);
    }
    vec_free(p);
}

// }}}

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
    atexit(repo_free);
    atexit(cache_free);

    switch (repo_init()) {
        case -4:
            die("relative path found in KISS_PATH");

        case -3:
            die("string error");

        case -2:
            die("failed to allocate memory");

        case -1:
            die("repository in KISS_PATH inaccessible: %s", strerror(errno));
    }

    switch (cache_init()) {
        case -4:
            die("cache directory not absolute or HOME unset");

        case -3:
            die("string error");

        case -2:
            die("failed to allocate memory");

        case -1:
            die("failed to create cache directory: %s", strerror(errno));
    }

    struct pkg **pkgs = 0;

    for (int i = 2; i < argc; i++) {
        vec_push(pkgs, pkg_init(argv[i]));     
    }

    for (size_t i = 0; i < vec_size(pkgs); i++) {
        switch (pkg_source(pkgs[i])) {
            case -3:
                msg("[%s] no sources, skipping", pkgs[i]->name);
                break;

            case -2:
                pkg_free_all(pkgs);
                die("[%s] invalid sources", pkgs[i]->name);

            case -1:
                pkg_free_all(pkgs);
                die("[%s] failed to open sources: %s", pkgs[i]->name, 
                    strerror(errno));
        }
    }

    pkg_free_all(pkgs);
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

    } else if (ARG(argv[1], "build") ||
               ARG(argv[1], "checksum") ||
               ARG(argv[1], "download")) {
        run_action(argc, argv);

    } else {
        run_extension(argv + 1);
    }

    return EXIT_SUCCESS;
}

