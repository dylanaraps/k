#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "util.h"

// Matches 'b' -> 'b' or 'build' -> 'build'.
// The first letter in each argument is compared first, only if this
// succeeds (and long form arguments are used) is strcmp() ever called.
#define ARG(a, n) ((a[0]) == (n[0]) && ((!a[1]) || strcmp(a, n) == 0))

static void version(void) {
    printf("kiss 0.0.1 (compiled %s %s)\n", __DATE__, __TIME__);
}

static void usage(void) {
    puts("kiss [a|b|c|d|i|l|r|s|u|v] [pkg]...");
    puts("alt        List and swap to alternatives");
    puts("build      Build a package");
    puts("checksum   Generate checksums");
    puts("download   Pre-download all sources");
    puts("install    Install a package");
    puts("list       List installed packages");
    puts("remove     Remove a package");
    puts("search     Search for a package");
    puts("update     Update the system");
    puts("version    Package manager version");
}

int main(int argc, char *argv[]) {
    if (argc < 2 || !argv[1] || !argv[1][0] ||
        argv[1][0] == '-' || argc > 1024) {
        usage();
        return EXIT_SUCCESS;

    } else if (ARG(argv[1], "search")) {
        //

    } else if (ARG(argv[1], "version")) {
        version();
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}

