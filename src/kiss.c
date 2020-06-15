#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#include "path.h"
#include "source.h"
#include "util.h"
#include "kiss.h"

#define msg(...) fprintf(stderr, __VA_ARGS__);

char *HOME, *CAC_DIR, *MAK_DIR, *PKG_DIR, *TAR_DIR, *SRC_DIR, *LOG_DIR, *BIN_DIR;
char **KISS_PATH;

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

    if (!strcmp(argv[1], "d") || !strcmp(argv[1], "download")) {
        for (int i = 2; i < argc; i++) {
            parse_sources(argv[i]);
        }

    } else if (!strcmp(argv[1], "l") || !strcmp(argv[1], "list")) {

    } else if (!strcmp(argv[1], "s") || !strcmp(argv[1], "search")) {
        for (int i = 2; i < argc; i++) {
            path_find(argv[i], 1);
        }

    } else if (!strcmp(argv[1], "v") || !strcmp(argv[1], "version")) {
        msg("3.0.3\n");
    }
}

int main (int argc, char *argv[]) {
    KISS_PATH = path_load();

    cache_init();
    curl_global_init(CURL_GLOBAL_ALL);

    args(argc, argv);

    return 0;
}
