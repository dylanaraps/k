#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "sha256.h"
#include "test.h"

static const unsigned char test1[SHA256_DIGEST_LENGTH] = {
    0xda, 0x8c, 0x87, 0x50, 0xe5, 0xe5, 0xba, 0x89, 0x9b, 0x48, 0x81,
    0x8c, 0x9d, 0xd7, 0x50, 0x1d, 0xa7, 0x47, 0x2d, 0xb9, 0x82, 0xb3,
    0x46, 0xea, 0x7e, 0x40, 0xb7, 0x6c, 0xfb, 0x62, 0x47, 0xaf,
};

static const char *test_sum = \
    "da8c8750e5e5ba899b48818c9dd7501da7472db982b346ea7e40b76cfb6247af";

int main(int argc, char *argv[]) {
    (void) argc;
    (void) argv;

    test_begin(__FILE__);

    unsigned char hash[SHA256_DIGEST_LENGTH];
    FILE *f = fopen("test/files/multi_line", "r");
    sha256_file(hash, f);
    fclose(f);

    for (size_t i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        test(hash[i] == test1[i]);
    }

    char hash_string[65];
    sha256_to_string(hash, hash_string);
    test(strcmp(hash_string, test_sum) == 0);

    return test_finish();
}

