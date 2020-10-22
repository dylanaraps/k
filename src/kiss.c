#include <stdio.h>

static void usage(char *arg0) {
    fputs(arg0, stdout);
    fputs(" [a|b|c|d|i|l|r|s|u|v] [pkg]...\n", stdout);

    puts("alternatives List and swap to alternatives");
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

int main(int argc, char *argv[]) {
    if (argc < 2 || !argv[1] || !argv[1][0] ||
        argv[1][0] == '-' || argc > 1024) {
        usage(argv[0]);
    }

    return 0;
}

