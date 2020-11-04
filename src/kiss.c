#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "action.h"
#include "download.h"
#include "error.h"
#include "list.h"
#include "str.h"

static void usage(char *arg0) {
    fputs(arg0, stdout);
    fputs(" [a|b|c|d|i|l|r|s|u|v] [pkg]...\n", stdout);

    puts("alt          List and swap to alternatives");
    puts("build        Build a package");
    puts("checksum     Generate checksums");
    puts("download     Pre-download all sources");
    puts("install      Install a package");
    puts("list         List installed packages");
    puts("remove       Remove a package");
    puts("search       Search for a package");
    puts("update       Update the system");
    puts("version      Package manager version");
}

static int run_extension(char *argv[]) {
    char ext[256] = "kiss-";

    strncat(ext, *argv, 255);
    execvp(ext, argv);

    err("failed to execute extension %s", ext);
    return -1;
}

static int run_search(int argc, char *argv[]) {
    (void) argc;
    (void) argv;

    return 0;
}

int main (int argc, char *argv[]) {
    int err = 0;

    if (argc < 2 || !argv[1] || !argv[1][0] || argv[1][0] == '-') {
        usage(argv[0]);

// Check if argument matches an action. True for b==build and build==build
// strcmp is only reached when both first characters match.
#define ARG(a, b) ((a[0]) == (b[0]) && ((!a[1]) || strcmp(a, b) == 0))

    } else if (ARG(argv[1], "version")) {
        puts("0.0.1");

    } else if (ARG(argv[1], "list")) {
        err = action_list(argc, argv);

    } else if (ARG(argv[1], "search")) {
        err = run_search(argc, argv);

    } else {
        err = run_extension(argv + 1);
    }

    return err;
}

