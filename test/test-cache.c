#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "cache.h"
#include "test.h"

static const char *test_path[] = {
    "test/test_hier/.cache/kiss/proc",
    "test/test_hier/kiss/proc",
};

int main(int argc, char *argv[]) {
    (void) argc;
    (void) argv;

    int err = test_begin(__FILE__);

    struct cache c;

    for (size_t i = 0; i < 2; i++) {
        setenv(i ? "XDG_CACHE_HOME" : "HOME", "./test/test_hier", 1);
        setenv(i ? "HOME" : "XDG_CACHE_HOME", "", 1);

        err = cache_init(&c); {
            test(err == 0);
            test(c.dir);
            test(c.fd[CAC_DIR]);
            test(fcntl(c.fd[CAC_DIR], F_GETFL) != -1 || errno != EBADF);

            for (size_t j = 0; j < CAC_DIR; j++) {
                test(c.fd[j]);
                test(fcntl(c.fd[j], F_GETFL) != -1 || errno != EBADF);
            }

            test(access(c.dir, F_OK) == 0);
            test(access(test_path[i], F_OK) == 0);

            size_t len_pre = buf_len(c.dir);
            buf_printf(&c.dir, "%s/%ld", test_path[i], getpid());
            int fd = open(c.dir + len_pre, O_RDONLY);
            buf_set_len(c.dir, len_pre);

            test(fcntl(fd, F_GETFL) != -1 || errno != EBADF);
            test(faccessat(fd, "build", F_OK, 0) == 0);
            test(faccessat(fd, "extract", F_OK, 0) == 0);
            test(faccessat(fd, "pkg", F_OK, 0) == 0);

            close(fd);
        }

        cache_clean(&c); {
            test(access(c.dir, F_OK) == -1);
        }

        cache_free(&c); {
            for (size_t j = 0; j < CAC_DIR; j++) {
                test(fcntl(c.fd[j], F_GETFL) == -1 && errno == EBADF);
            }
        }
    }

    setenv("XDG_CACHE_HOME", "", 1);
    setenv("HOME", "", 1);

    err = cache_init(&c); {
        test(err == -1);
    }

    cache_free(&c);

    return test_finish();
}

