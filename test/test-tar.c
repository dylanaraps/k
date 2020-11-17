#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "file.h"
#include "tar.h"
#include "test.h"

int main(int argc, char *argv[]) {
    (void) argc;
    (void) argv;

    int ret = test_begin(__FILE__);

    ret = tar_create("test/test_hier/bin", "test.tar", TAR_NONE); {
        test(ret == 0);
        test(access("test.tar", F_OK) == 0);

        ret = tar_extract("test.tar"); {
            test(ret == 0);
            test(access("test_hier/bin/kiss-test", X_OK) == 0);
            rm_rf("test_hier");
            test(access("test_hier", F_OK) == -1 && errno == ENOENT);
            remove("test.tar");
        }
    }

    ret = tar_create("test/test_hier/bin", "test.tar.xz", TAR_XZ); {
        test(ret == 0);
        test(access("test.tar.xz", F_OK) == 0);

        ret = tar_extract("test.tar.xz"); {
            test(ret == 0);
            test(access("test_hier/bin/kiss-test", X_OK) == 0);
            rm_rf("test_hier");
            test(access("test_hier", F_OK) == -1 && errno == ENOENT);
            remove("test.tar.xz");
        }
    }

    ret = tar_create("test/test_hier/bin", "test.tar.gz", TAR_GZ); {
        test(ret == 0);
        test(access("test.tar.gz", F_OK) == 0);

        ret = tar_extract("test.tar.gz"); {
            test(ret == 0);
            test(access("test_hier/bin/kiss-test", X_OK) == 0);
            rm_rf("test_hier");
            test(access("test_hier", F_OK) == -1 && errno == ENOENT);
            remove("test.tar.gz");
        }
    }

    ret = tar_create("test/test_hier/bin", "test.tar.bz2", TAR_BZ2); {
        test(ret == 0);
        test(access("test.tar.bz2", F_OK) == 0);

        ret = tar_extract("test.tar.bz2"); {
            test(ret == 0);
            test(access("test_hier/bin/kiss-test", X_OK) == 0);
            rm_rf("test_hier");
            test(access("test_hier", F_OK) == -1 && errno == ENOENT);
            remove("test.tar.bz2");
        }
    }

    return test_finish();
}

