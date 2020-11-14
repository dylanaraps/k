#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include "action.h"
#include "test.h"

static const char *pkgs[] = {
    "",
    "",
    "zlib",
    "samurai"
};

int main(int argc, char *argv[]) {
    (void) argc;
    (void) argv;

    test_begin(__FILE__);
    struct state *s = 0;

    s = state_init(4, (char **) pkgs, STATE_ALL); {
        test(s);
        test(s->opt == STATE_ALL);
        test(s->mem);
        test(buf_len(s->mem) == 0);
        test(buf_cap(s->mem) == 1024);
        test(strcmp(s->pkgs[0]->name, "zlib") == 0);
        test(strcmp(s->pkgs[1]->name, "samurai") == 0);
        test(s->repos);
        test(s->repos[0]->path[0]);

        for (size_t i = 0; i < CAC_DIR; i++) {
            test(fcntl(s->cache.fd[i], F_GETFL) != -1 || errno != EBADF);
        }
    }
    state_free(s);

    s = state_init(0, NULL, STATE_MEM); {
        test(s);
        test(s->opt == STATE_MEM);
        test(s->mem);
        test(buf_len(s->mem) == 0);
        test(buf_cap(s->mem) == 1024);
        test(!s->pkgs);
        test(!s->repos);
        test(!s->cache.dir);
    }
    state_free(s);

    s = state_init(0, NULL, STATE_REPO); {
        test(s);
        test(s->opt == STATE_REPO);
        test(!s->mem);
        test(!s->pkgs);
        test(s->repos);
        test(s->repos[0]->path[0]);
        test(!s->cache.dir);
    }
    state_free(s);

    s = state_init(0, NULL, STATE_CACHE); {
        test(s);
        test(s->opt == STATE_CACHE);
        test(!s->mem);
        test(!s->pkgs);
        test(!s->repos);
        test(s->cache.dir);

        for (size_t i = 0; i < CAC_DIR; i++) {
            test(fcntl(s->cache.fd[i], F_GETFL) != -1 || errno != EBADF);
        }
    }
    state_free(s);

    s = state_init(4, (char **) pkgs, STATE_PKG); {
        test(s);
        test(s->opt == STATE_PKG);
        test(!s->mem);
        test(s->pkgs);
        test(!s->repos);
        test(!s->cache.dir);
        test(strcmp(s->pkgs[0]->name, "zlib") == 0);
        test(strcmp(s->pkgs[1]->name, "samurai") == 0);
        test(s->pkgs[0]->repo_fd == 0);
        test(s->pkgs[1]->repo_fd == 0);
    }
    state_free(s);

    s = state_init(4, (char **) pkgs, STATE_PKG | STATE_CACHE); {
        test(s);
        test(s->opt == (STATE_PKG | STATE_CACHE));
        test(!s->mem);
        test(s->pkgs);
        test(!s->repos);
        test(s->cache.dir);
        test(strcmp(s->pkgs[0]->name, "zlib") == 0);
        test(strcmp(s->pkgs[1]->name, "samurai") == 0);
        test(s->pkgs[0]->repo_fd == 0);
        test(s->pkgs[1]->repo_fd == 0);
    }
    state_free(s);

    s = state_init(4, (char **) pkgs,
            STATE_PKG | STATE_CACHE | STATE_PKG_CACHE); {
        test(s);
        test(s->opt == (STATE_PKG | STATE_CACHE | STATE_PKG_CACHE));
        test(!s->mem);
        test(s->pkgs);
        test(!s->repos);
        test(s->cache.dir);
        test(s->pkgs[0]->repo_fd == 0);
        test(s->pkgs[1]->repo_fd == 0);
        test(strcmp(s->pkgs[0]->name, "zlib") == 0);
        test(strcmp(s->pkgs[1]->name, "samurai") == 0);
    }
    state_free(s);

    s = state_init(4, (char **) pkgs,
            STATE_PKG | STATE_REPO | STATE_PKG_REPO); {
        test(s);
        test(s->opt == (STATE_PKG | STATE_REPO | STATE_PKG_REPO));
        test(!s->mem);
        test(s->pkgs);
        test(s->repos);
        test(!s->cache.dir);
        test(strcmp(s->pkgs[0]->name, "zlib") == 0);
        test(strcmp(s->pkgs[1]->name, "samurai") == 0);
        test(s->pkgs[0]->repo_fd != 0);
        test(s->pkgs[1]->repo_fd != 0);
        test(fcntl(s->pkgs[0]->repo_fd, F_GETFL) != -1 || errno != EBADF);
        test(fcntl(s->pkgs[1]->repo_fd, F_GETFL) != -1 || errno != EBADF);
    }
    state_free(s);

    return test_finish();
}

