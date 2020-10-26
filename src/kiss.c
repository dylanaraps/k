#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "download.h"
#include "str.h"
#include "util.h"
#include "vec.h"

struct pkg {
    str *name;
    str *repo;
};

// Holds the list of repositories with the database directory tacked on
// the end. This variable should not be directly accessed. Use **repos
// which stores pointers to each repository in this string.
static str *KISS_PATH = 0;

// A vector holding each repository. Points to slices of memory from
// KISS_PATH above. This vector must be freed. Its elements on the other
// hand are automatically freed with KISS_PATH.
static char **repos = 0;

// A vector holding the package queue.
static struct pkg **pkgs = 0;

// Holds the cache directory + /proc/ + <pid>.
static str *cache_dir = 0;

// repo {{{

static void repo_free(void) {
    str_free(&KISS_PATH);
    vec_free(repos);
}

static void repo_init(void) {
    KISS_PATH = str_init_die(512);

    str_push_s(&KISS_PATH, xgetenv("KISS_PATH", ""));
    str_push_c(&KISS_PATH, ':');
    str_push_s(&KISS_PATH, xgetenv("KISS_ROOT", "/"));
    str_push_l(&KISS_PATH, "var/db/kiss/installed", 21);

    if (KISS_PATH->err != STR_OK) {
        die("failed to read KISS_PATH");
    }

    for (char *t = strtok(KISS_PATH->buf, ":"); t; t = strtok(0, ":")) {
        if (t[0] != '/') {
            die("relative path found in KISS_PATH");
        }

        if (access(t, F_OK) != 0) {
            die("error found in KISS_PATH '%s': %s", t, strerror(errno));
        }

        vec_push(repos, path_normalize(t));
    }
}

static str* repo_find(const char *name) {
    str *repo = str_init_die(0);

    for (size_t i = 0; i < vec_size(repos); i++) {
        str_push_s(&repo, repos[i]);   
        str_push_c(&repo, '/');
        str_push_s(&repo, name);
        str_push_c(&repo, '/');

        if (access(repo->buf, F_OK) == 0) {
            return repo;

        } else if (errno != ENOENT) {
            die("failed to stat package '%s': %s", name, strerror(errno));
        }

        str_undo_l(&repo, repo->len);
    }

    die("package '%s' not in any repository", name);
}

// }}}

// cache {{{

static void cache_free(void) {
    str_free(&cache_dir);
}

static void cache_get_base(str **s) {
    str_push_s(s, getenv("XDG_CACHE_HOME"));

    if ((*s)->err != STR_OK) {
        (*s)->err = STR_OK;

        str_push_s(s, getenv("HOME"));

        if ((*s)->err != STR_OK) {
            die("HOME is unset");
        }

        str_rstrip(s, '/');
        str_push_l(s, "/.cache", 7);
    }

    if ((*s)->buf[0] != '/') {
        die("Cache directory is relative '%s'", (*s)->buf);
    }

    str_rstrip(s, '/');
    str_push_l(s, "/kiss/", 6);
}

static void cache_create(str *s, const char *p, size_t l) {
    str_push_l(&s, p, l);

    if (s->err == STR_OK) {
        mkdir_die(s->buf);
    }

    str_undo_l(&s, l);
}

static void cache_init(void) {
    cache_dir = str_init_die(256);    

    // Attempt to create the system's cache directory and fail if one 
    // or more path components does not exist. This is a naive safegaurd
    // as we will only continue if XDG_CACHE_HOME or HOME/.cache already
    // exists in the system.
    cache_get_base(&cache_dir);

    // Permanent cache directories.
    cache_create(cache_dir, "bin", 3);
    cache_create(cache_dir, "sources", 7);

    str_push_l(&cache_dir, "proc", 4);
    str_printf(&cache_dir, "/%u/", getpid());

    // Temporary cache directories.
    cache_create(cache_dir, "build", 5);
    cache_create(cache_dir, "extract", 7);
    cache_create(cache_dir, "pkg", 3);

    if (cache_dir->err != STR_OK) {
        die("failed to create cache"); 
    }
}

// }}}

// pkgs {{{

static void pkg_free(struct pkg **p) {
    str_free(&(*p)->name); 
    str_free(&(*p)->repo); 
    free(*p);
    *p = NULL;
}

static void pkg_source(struct pkg *p) {
    str_push_l(&p->repo, "sources", 7);
    FILE *f = fopen(p->repo->buf, "r");
    str_undo_l(&p->repo, 7);

    if (!f) {
        switch (errno) {
            case ENOENT:
                msg("[%s] no sources file, skipping", p->name->buf);
                return;

            default:
                die("[%s] failed to open sources file: %s", 
                        p->name->buf, strerror(errno));
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
            die("[%s] invalid sources file", p->name->buf);
        }

        if (strncmp(src, "git+", 4) == 0) {
            die("[%s] found git %s", p->name->buf, src);    

        } else if (strstr(src, "://")) {
            str_undo_l(&tmp, tmp->len);
            cache_get_base(&tmp);

            str_push_l(&tmp, "sources/", 8);
            str_push_l(&tmp, p->name->buf, p->name->len);
            str_push_c(&tmp, '/');

            if (des) {
                str_push_s(&tmp, des);
                str_rstrip(&tmp, '/');
                str_push_c(&tmp, '/');
            }

            if (tmp->err == STR_OK) {
                mkdir_die(tmp->buf);
            }

            char *basename = strrchr(src, '/');

            if (!basename) {
                die("[%s] invalid source found '%s'", p->name->buf, src);
            }

            str_push_s(&tmp, basename + 1);

            if (access(tmp->buf, F_OK) == 0) {
                msg("[%s] found cache %s", p->name->buf, basename + 1);

            } else if (errno == ENOENT) {
                msg("[%s] downloading %s", p->name->buf, basename + 1);

                if (source_download(src, tmp->buf) != 0) {
                    die("[%s] failed to download source %s", p->name->buf, src);
                }

            } else {
                die("[%s] error accessing file %s: %s", 
                    p->name->buf, tmp->buf, strerror(errno));
            }

        } else {
            str_push_s(&p->repo, src);

            if (access(p->repo->buf, F_OK) == 0) {
                msg("[%s] found relative %s", p->name->buf, src);

            } else if (access(src, F_OK) == 0) {
                msg("[%s] found absolute %s", p->name->buf, src);

            } else {
                die("[%s] source not found %s", p->name->buf, src);
            }

            str_undo_s(&p->repo, src);
        }
    }

    free(line);
    fclose(f);
}

static struct pkg *pkg_init(const char *name) {
    struct pkg *p = malloc(sizeof *p);    

    if (!p) {
        die("failed to allocate memory");
    }

    p->name = str_init_die(0);
    p->repo = repo_find(name);

    str_push_s(&p->name, name);

    if (p->name->err != STR_OK ||
        p->repo->err != STR_OK) {
        pkg_free(&p);
        die("failed to create strings");
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
    cache_init();

    for (int i = 2; i < argc; i++) {
        vec_push(pkgs, pkg_init(argv[i]));     
    }

    for (size_t i = 0; i < vec_size(pkgs); i++) {
        printf("%s: %s\n", pkgs[i]->name->buf, pkgs[i]->repo->buf);
    }

    switch (argv[1][0]) {
        case 'b': 
            break;

        case 'c': 
            break;

        case 'd': 
            for (size_t i = 0; i < vec_size(pkgs); i++) {
                pkg_source(pkgs[i]);
            }
            break;
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

