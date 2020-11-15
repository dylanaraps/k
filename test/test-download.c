#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "download.h"
#include "sha256.h"
#include "test.h"

static const unsigned char file_hash[SHA256_LEN] = {
    0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14, 0x9a, 0xfb, 0xf4,
    0xc8, 0x99, 0x6f, 0xb9, 0x24, 0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b,
    0x93, 0x4c, 0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55,
};

int main(int argc, char *argv[]) {
    (void) argc;
    (void) argv;

#ifdef USE_CURL
    int ret = test_begin(__FILE__);

    FILE *dest = fopen("test/team.txt", "w");

    // TODO: Upload file to website specifically for this purpose.
    ret = source_download("https://k1ss.org/team.txt", dest); {
        test(ret == 0);
        test(access("test/team.txt", F_OK) == 0);
    }

    unsigned char hash[SHA256_LEN];
    sha256_file(hash, dest);

    for (size_t i = 0; i < SHA256_LEN; i++) {
        test(hash[i] == file_hash[i]);
    }

    remove("test/team.txt");
    fclose(dest);

    source_curl_cleanup();

    return test_finish();
#endif

    return 0;
}

