#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#include "find.h"
#include "source.h"

#define msg(...) fprintf(stderr, __VA_ARGS__);

void args(int argc, char *argv[]) {
    if (argc == 1) {
        msg("kiss [a|b|c|i|l|r|s|u|v] [pkg]...\n");
        msg("alternatives: List and swap to alternatives\n");
        msg("build:        Build a package\n");
        msg("checksum:     Generate checksums\n");
        msg("download:     Pre-download all sources\n");
        msg("install:      Install a package\n");
        msg("list:         List installed packages\n");
        msg("remove:       Remove a package\n");
        msg("search:       Search for a package\n");
        msg("update:       Check for updates\n");
        msg("version:      Package manager version\n");

        exit(0);
    }

    if (argc == 2) {
        msg("! kiss %s requires an argument\n", argv[1]);
        exit(1);
    }

    if (!strcmp(argv[1], "d") || !strcmp(argv[1], "download")) {
        source_download("https://zlib.net/zlib-1.2.11.tar.gz");
    }

    if (!strcmp(argv[1], "s") || !strcmp(argv[1], "search")) {
        msg("%s\n", path_query(argv[2]));
    }
}

int main (int argc, char *argv[]) {
    curl_global_init(CURL_GLOBAL_ALL);
    args(argc, argv);

    return 0;
}
