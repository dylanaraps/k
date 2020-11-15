#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "tar.h"
#include "test.h"

int main(int argc, char *argv[]) {
    (void) argc;
    (void) argv;

    int ret = test_begin(__FILE__);

    ret = tar_extract("test/test_hier/bin/test.tar", 0); {
        test(ret == 0);
        test(access("kiss-test", X_OK) == 0);
        remove("kiss-test");
    }

    ret = tar_extract("test/test_hier/bin/test.tar.gz", 0); {
        test(ret == 0);
        test(access("kiss-test", X_OK) == 0);
        remove("kiss-test");
    }

    ret = tar_extract("test/test_hier/bin/test.tar.xz", 0); {
        test(ret == 0);
        test(access("kiss-test", X_OK) == 0);
        remove("kiss-test");
    }

    ret = tar_extract("test/test_hier/bin/test.tar.bz2", 0); {
        test(ret == 0);
        test(access("kiss-test", X_OK) == 0);
        remove("kiss-test");
    }

    return test_finish();
}

