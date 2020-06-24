#define _POSIX_C_SOURCE 200809L
#include <stdio.h>  /* FILE */
#include <limits.h> /* LINE_MAX */
#include <string.h> /* strchr */
#include <fcntl.h>  /* O_RDONLY */

#include "log.h"
#include "util.h"
#include "version.h"

void pkg_version(package *pkg) {
    char *line = 0;
    FILE *file;
    int err;

    file = fopenat(pkg->path, "version", "r");

    if (!file) {
        die("[%s] Version file does not exist", pkg->name);
    }

    err = getline(&line, &(size_t){0}, file);

    if (err == -1) {
        die("[%s] Failed to read version file", pkg->name);
    }

    split_in_two(line, " 	\r\n", &pkg->ver, &pkg->rel);

    if (!pkg->ver || !pkg->rel[0]) {
        die("[%s] Failed to read or invalid version file", pkg->name);
    }

    free(line);
    fclose(file);
}
