#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cache.h"
#include "test.h"

static void cache_check(int err, const char *path) {
    struct cache c;

    err = cache_init(&c); {
        test(err == 0);
        test(c.dir);
        test(c.fd[CAC_DIR]);
        test(fcntl(c.fd[CAC_DIR], F_GETFL) != -1 || errno != EBADF);

        for (size_t j = 0; j < CAC_DIR; j++) {
            test(fcntl(c.fd[j], F_GETFL) != -1 || errno != EBADF);
        }

        test(access(c.dir, F_OK) == 0);
        test(access(path, F_OK) == 0);

        size_t len_pre = buf_len(c.dir);
        buf_printf(&c.dir, "%s/%ld", path, getpid());
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

int main(int argc, char *argv[]) {
    (void) argc;
    (void) argv;

    int err = test_begin(__FILE__);

    // check KISS_TMPDIR
    setenv("KISS_TMPDIR", "test/test_hier", 1);
    setenv("XDG_CACHE_HOME", "", 1);
    setenv("HOME", "", 1);
    cache_check(err, "test/test_hier/kiss/proc");

    // check XDG_CACHE_HOME
    setenv("KISS_TMPDIR", "", 1);
    setenv("XDG_CACHE_HOME", "test/test_hier", 1);
    setenv("HOME", "", 1);
    cache_check(err, "test/test_hier/kiss/proc");

    // check HOME
    setenv("KISS_TMPDIR", "", 1);
    setenv("XDG_CACHE_HOME", "", 1);
    setenv("HOME", "test/test_hier", 1);
    cache_check(err, "test/test_hier/.cache/kiss/proc");

    // check invalid
    setenv("KISS_TMPDIR", "", 1);
    setenv("XDG_CACHE_HOME", "", 1);
    setenv("HOME", "", 1);
    struct cache c;
    err = cache_init(&c); {
        test(err == -1);
    }
    buf_free(&c.dir);

    return test_finish();
}

