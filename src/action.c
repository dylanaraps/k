#include <stdlib.h>

#include "arr.h"
#include "cache.h"
#include "error.h"
#include "pkg.h"
#include "repo.h"
#include "action.h"

struct state *state_init(int argc, char *argv[], int opt) {
    struct state *s = malloc(sizeof *s);

    if (!s) {
        return NULL;
    }

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

        if (repo_open_PATH(s->repos, getenv("KISS_PATH")) < 0) {
            goto error;
        }
    }

    if (opt & STATE_PKG) {
        if (!(s->pkgs = arr_alloc(0, (size_t) argc))) {
            goto error;
        }

        for (int i = 2; i < argc; i++) {
            if (strstr(argv[i], "/")) {
                err("invalid argument '%s'", argv[i]);
                goto error;
            }

            pkg *n = pkg_alloc(argv[i]);

            if (!n) {
                goto error;
            }

            if (opt & STATE_PKG_REPO) {
                if ((n->repo_fd = repo_find_pkg(s->repos, argv[i])) == -1) {
                    err_no("package '%s' not in any repository", argv[i]);
                    pkg_free(n);
                    goto error;
                }
            }

            if (opt & STATE_PKG_CACHE) {
                if (cache_init_pkg(&s->cache, argv[i]) < 0) {
                    pkg_free(n);
                    goto error;
                }
            }

            arr_push_b(s->pkgs, n);
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

    if (s->pkgs) {
        pkg_free_all(s->pkgs);
    }

    if (s->repos) {
        repo_free_all(s->repos);
    }

    if (s->cache.dir) {
        cache_free(&s->cache);
    }

    if (s->mem) {
        buf_free(&s->mem);
    }

    free(s);
}

