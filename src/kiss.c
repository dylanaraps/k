#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdarg.h>
#include <string.h>

#include "vec.h"
#include "pkg.h"

#define die(...) _m(":(", __FILE__, __LINE__, __VA_ARGS__),exit(1)
#define msg(...) _m("OK", __FILE__, __LINE__, __VA_ARGS__)

static void _m(const char* t, const char *f, const int l, const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);

    printf("[%s] (%s:%d) ", t, f, l);
    vprintf(fmt, args);
    printf("\n");

    va_end(args);
}

static char *xstrdup(const char *s) {
    char *p = strdup(s);

    if (!p) {
        die("Failed to allocate memory");
    }

    return p;
}

static char *xgetenv(const char *s) {
    char *p = getenv(s);

    if (!p || !p[0]) {
        die("%s must be set", s);
    }

    return xstrdup(p);
}

static char **repo_init(void) {
    char *tmp;
    char *env;
    char **repos = NULL;
    int i = 0;

    env = xgetenv("KISS_PATH");

    if (!strchr(env, '/')) {
        die("Invalid KISS_PATH");
    }

    while ((tmp = strtok(i++ ? NULL : env, ":"))) {
        vec_push_back(repos, xstrdup(tmp));
    }
    vec_push_back(repos, "/var/db/kiss/installed");

    free(env);

    return repos;
}

static package pkg_new(char *name) {
    package new;

    if (!name) {
        die("Package name is null");
    }

    new = (package) {
        .name = name,
    };

    return new;
}

static void sig_init(void) {
    struct sigaction sa = {
        .sa_handler = exit,
    };

    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGINT);

    sigaction(SIGINT,  &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
}

static void usage(void) {
    printf("kiss [b|c|d|l|s|v] [pkg]...\n");
    printf("build:        Build a package\n");
    printf("checksum:     Generate checksums\n");
    printf("download:     Pre-download all sources\n");
    printf("list:         List installed packages\n");
    printf("search:       Search for a package\n");
    printf("version:      Package manager version\n");

    exit(0);
}

int main (int argc, char *argv[]) {
    package *pkgs = NULL;
    char  **repos = NULL;

    sig_init();

    if (argc == 1) {
        usage();
    }

    for (int i = 2; i < argc; i++) {
        vec_push_back(pkgs, pkg_new(argv[i]));
    }

    repos = repo_init();

    for (size_t i = 0; i < vec_size(repos); ++i) {
        printf("v[%lu] = %s\n", i, repos[i]);
    }

    switch (argv[1][0]) {
        case 'b':
        case 'c':
        case 'd':
        case 'l':
        case 's':
            break;

        default:
            usage();
    }

    vec_free(repos);
    vec_free(pkgs);
}
