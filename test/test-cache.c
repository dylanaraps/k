#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "cache.h"
#include "test.h"

int main(int argc, char *argv[]) {
    (void) argc;
    (void) argv;

    int err = test_begin(__FILE__);

    struct cache c;

    err = cache_init(&c); {
        test(err == 0);
        test(c.dir);
        test(c.fd[CAC_DIR]);
        test(fcntl(c.fd[CAC_DIR], F_GETFL) != -1 || errno != EBADF);

        for (size_t i = 0; i < CAC_DIR; i++) {
            test(c.fd[i]);
            test(fcntl(c.fd[i], F_GETFL) != -1 || errno != EBADF);
        }

        test(access(c.dir, F_OK) == 0);
    }

    cache_clean(&c); {
        test(access(c.dir, F_OK) == -1);
    }

    cache_free(&c); {
        for (size_t i = 0; i < CAC_DIR; i++) {
            test(fcntl(c.fd[i], F_GETFL) == -1 && errno == EBADF);
        }
    }

    return test_finish();
}

