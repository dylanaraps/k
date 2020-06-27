#define _POSIX_C_SOURCE 200809L
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "vec.h"
#include "pkg.h"

#define pkg_iter(p) for (size_t i = 0; i < vec_size(p); ++i)

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

static void xsnprintf(char *str, size_t size, const char *fmt, ...) {
    va_list va;
    unsigned int err;

    va_start(va, fmt);
    err = vsnprintf(str, size, fmt, va);
    va_end(va);

    if (err < 1) {
        die("snprintf failed to construct string");
    }

    if (err > size) {
        die("snprintf result exceeds buffer size");
    }
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

static FILE *fopenat(const char *d, const char *f, const int o, const char *m) {
    int dfd;
    int ffd;

    dfd = open(d, O_SEARCH);

    if (dfd == -1) {
        return NULL;
    }

    ffd = openat(dfd, f, o, 0644);
    close(dfd);

    if (ffd == -1) {
        return NULL;
    }

    /* fclose() by caller also closes the open()'d fd here */
    return fdopen(ffd, m);
}

static char **repo_init(void) {
    char *tmp;
    char *env;
    char **repos = NULL;
    int i = 0;

    env = xgetenv("KISS_PATH");

    while ((tmp = strtok(i++ ? NULL : env, ":"))) {
        vec_push_back(repos, xstrdup(tmp));
    }
    vec_push_back(repos, "/var/db/kiss/installed");

    free(env);

    return repos;
}

static char *pkg_find(const char *name, char **repos, const int all) {
    FILE *f;

    for (size_t j = 0; j < vec_size(repos); ++j) {
        if ((f = fopenat(repos[j], name, O_DIRECTORY, "r"))) {
            fclose(f);

            if (!all) {
                return repos[j];
            }

            printf("%s/%s\n", repos[j], name);
        }
    }

    if (!all) {
        die("Package '%s' not in any repository", name);
    }

    return NULL;
}

static void pkg_version(package *pkg, char *repo) {
    FILE *file;
    char *ver_f;
    size_t ver_l;

    /* 10 == '/' + '/version' + '\0' */
    ver_l = strlen(repo) + strlen(pkg->name) + 10;
    ver_f = malloc(ver_l);

    xsnprintf(ver_f, ver_l, "%s/%s/version", repo, pkg->name);

    if (!(file = fopen(ver_f, "r"))) {
        die("[%s], Version file not found", pkg->name);
    }
    free(ver_f);

    if ((getline(&pkg->ver, &(size_t){0}, file)) == -1) {
        die("[%s] Failed to read version file", pkg->name);
    }
    fclose(file);

    pkg->ver[strcspn(pkg->ver, "\n")] = 0;
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

static void pkg_list(package *pkg) {
    char *db = "/var/db/kiss/installed";
    struct dirent **list;
    int err;

    if (vec_size(pkg) == 0) {
        if ((err = scandir(db, &list, NULL, alphasort)) == -1) {
            die("Installed database not accessible");
        }

        for (int i = 0; i < err; i++) {
            if (list[i]->d_name[0] != '.' && list[i]->d_name[2]) {
                vec_push_back(pkg, pkg_new(list[i]->d_name));
            }
        }

        free(list);
    }

    pkg_iter(pkg) {
        pkg_version(&pkg[i], db);

        printf("%s %s\n", pkg[i].name, pkg[i].ver);
    }
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

    repos = repo_init();

    for (int i = 2; i < argc; i++) {
        vec_push_back(pkgs, pkg_new(argv[i]));
    }

    switch (argv[1][0]) {
        case 'b':
        case 'c':
        case 'd':
            pkg_iter(pkgs) {
                pkgs[i].path = pkg_find(pkgs[i].name, repos, 0);
            }
            break;

        case 'l':
            pkg_list(pkgs);
            break;

        case 's':
            pkg_iter(pkgs) {
                pkg_find(pkgs[i].name, repos, 1);
            }
            break;

        default:
            usage();
    }

    switch (argv[1][0]) {
        case 'b':
        case 'c':
        case 'd':
            break;
    }

    vec_free(repos);
    vec_free(pkgs);
}
