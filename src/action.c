#include <stdlib.h>

#include "action.h"
#include "arr.h"
#include "cache.h"
#include "error.h"
#include "pkg.h"
#include "repo.h"

int state_init_pkg(struct state *s, char *p) {
    if (strstr(p, "/")) {
        err("invalid argument '%s'", p);
        return -1;
    }

    pkg *n = pkg_alloc(p);

    if (!n) {
        return -1;
    }

    if (s->opt & STATE_PKG_REPO) {
        if ((n->repo_fd = repo_find_pkg(s->repos, p)) == -1) {
            err_no("package '%s' not in any repository", p);
            pkg_free(n);
            return -1;
        }
    }

    if (s->opt & STATE_PKG_CACHE) {
        if (cache_init_pkg(&s->cache, p) < 0) {
            pkg_free(n);
            return -1;
        }
    }

    arr_push_b(s->pkgs, n);
    return 0;
}

static int state_init_repo_pwd(struct state *s) {
    if (buf_push_s(&s->mem, getenv("PWD")) == -EINVAL) {
        return -1;
    }

    size_t bn = buf_scan_rev(&s->mem, '/');

    buf_set_len(s->mem, bn - 1);
    buf_push_c(&s->mem, ':');
    buf_push_s(&s->mem, getenv("KISS_PATH"));

    if (setenv("KISS_PATH", s->mem, 1) == -1) {
        err_no("failed to set KISS_PATH");
        return -1;
    }

    buf_set_len(s->mem, 0);
    return 0;
}

static int state_init_pkg_pwd(struct state *s) {
    if (buf_push_s(&s->mem, getenv("PWD")) == -EINVAL) {
        return -1;
    }

    size_t bn = buf_scan_rev(&s->mem, '/');

    if (state_init_pkg(s, s->mem + bn) < 0) {
        return -1;
    }

    buf_set_len(s->mem, 0);
    return 0;
}

struct state *state_init(int argc, char *argv[], int opt) {
    struct state *s = calloc(sizeof *s, 1);

    if (!s) {
        return NULL;
    }

    s->opt = opt;

    if (opt & STATE_MEM) {
        if (!(s->mem = buf_alloc(0, 1024))) {
            goto error;
        }
    }

    if (opt & STATE_CACHE) {
        if (cache_init(&s->cache) < 0) {
            goto error;
        }
    }

    if (opt & STATE_REPO) {
        if (!(s->repos = arr_alloc(0, 12))) {
            goto error;
        }

        if (argc == 2 && state_init_repo_pwd(s) < 0) {
            goto error;
        }

        if (repo_open_PATH(s->repos, getenv("KISS_PATH")) < 0) {
            goto error;
        }
    }

    if (opt & STATE_PKG) {
        if (!(s->pkgs = arr_alloc(0, (size_t) argc))) {
            goto error;
        }

        if ((opt & STATE_PKG_PWD) && argc == 2) {
            if (state_init_pkg_pwd(s) < 0) {
                goto error;
            }
        }

        for (int i = 2; i < argc; i++) {
            if (state_init_pkg(s, argv[i]) < 0) {
                goto error;
            }
        }
    }

    if (opt & STATE_ARGV) {
        if (!(s->argv = arr_alloc(0, (size_t) argc))) {
            goto error;
        }

        for (int i = 2; i < argc; i++) {
            arr_set_end(s->argv, argv[i]);
        }
    }

    return s;

error:
    state_free(s);
    return NULL;
}

void state_free(struct state *s) {
    if (!s) {
        return;
    }

    if (s->opt & STATE_ARGV) {
        arr_free(s->argv);
    }

    if (s->opt & STATE_PKG) {
        pkg_free_all(s->pkgs);
    }

    if (s->opt & STATE_REPO) {
        repo_free_all(s->repos);
    }

    if (s->opt & STATE_CACHE) {
        cache_free(&s->cache);
    }

    if (s->opt & STATE_MEM) {
        buf_free(&s->mem);
    }

    free(s);
}

