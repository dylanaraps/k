#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "sha256.h"
#include "test.h"

static const unsigned char test1[SHA256_LEN] = {
    0x77, 0xd1, 0x02, 0x4b, 0xf4, 0xf9, 0x7c, 0x32, 0xf8, 0x15, 0xda,
    0x89, 0x6f, 0x7a, 0xbe, 0xec, 0xeb, 0x49, 0x21, 0x54, 0x89, 0x15,
    0x58, 0xf3, 0x3e, 0xde, 0x29, 0x95, 0x8b, 0xb5, 0x2b, 0x0f,
};

static const char *test_sum = \
    "77d1024bf4f97c32f815da896f7abeeceb492154891558f33ede29958bb52b0f";

int main(int argc, char *argv[]) {
    (void) argc;
    (void) argv;

    test_begin(__FILE__);

    unsigned char hash[SHA256_LEN];
    FILE *f = fopen("test/test_hier/repo/core/zlib/version", "r");
    sha256_file(hash, f);
    fclose(f);

    for (size_t i = 0; i < SHA256_LEN; i++) {
        test(hash[i] == test1[i]);
    }

    char hash_string[65];
    sha256_to_string(hash, hash_string);
    test(strcmp(hash_string, test_sum) == 0);

    return test_finish();
}

