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

// A vector holding the package queue.
static struct pkg **pkgs = 0;

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
        switch (errno) {
            case ENOENT:
                return -3;

            default:
                return -1;
        }
    }

    char *line = 0;
    str *tmp = str_init_die(256);

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

// }}}


static void exit_handler(void) {
    repo_free(); 
    cache_free();

    for (size_t i = 0; i < vec_size(pkgs); i++) {
        pkg_free(&pkgs[i]);
    }

    vec_free(pkgs);
}

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
    str *ext = str_init_die(32);

    str_push_l(&ext, "kiss-", 5);
    str_push_s(&ext, *argv);

    if (ext->err != STR_OK) {
        die("failed to construct string 'kiss-%s'", *argv);
    }

    execvp(ext->buf, argv);
    die("failed to execute extension kiss-%s", *argv);
}

static int run_action(int argc, char *argv[]) {
    repo_init();

    switch (cache_init()) {
        case -4:
            die("failed to allocate memory");

        case -3:
            die("cache directory not absolute or HOME unset");

        case -2:
            die("string error");

        case -1:
            die("failed to create cache directory: %s", strerror(errno));
    }

    for (int i = 2; i < argc; i++) {
        vec_push(pkgs, pkg_init(argv[i]));     
    }

    if (argv[1][0] == 'd' || argv[1][0] == 'c' || argv[1][0] == 'b') {
        for (size_t i = 0; i < vec_size(pkgs); i++) {
            switch (pkg_source(pkgs[i])) {
                case -1:
                    die("[%s] failed to open sources file: %s", 
                        pkgs[i]->name, strerror(errno));

                case -2:
                    die("[%s] invalid/empty sources file", pkgs[i]->name);

                case -3:
                    msg("[%s] no sources file, skipping", pkgs[i]->name);
            }
        }
    }

    return EXIT_SUCCESS;
}

int main (int argc, char *argv[]) {
// Check equality of arguments. If both first characters match, the comparison
// continues, if not the strcmp() call is skipped entirely. This matches
// 'b' -> 'b' or 'build' -> 'build'.
#define ARG(a, n) ((a[0]) == (n[0]) && ((!a[1]) || strcmp(a, n) == 0))

    atexit(exit_handler);

    if (argc < 2 || !argv[1] || !argv[1][0] ||
        argv[1][0] == '-' || argc > 1024) {
        usage(argv[0]);

    } else if (ARG(argv[1], "version")) {
        puts("0.0.1");

    } else if (ARG(argv[1], "build")    ||
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

