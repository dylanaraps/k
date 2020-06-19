#define _POSIX_C_SOURCE 200809L
#include <stdio.h>  /* FILE */
#include <unistd.h> /* chdir */
#include <limits.h> /* LINE_MAX */

#include "log.h"
#include "version.h"

void pkg_version(package *pkg) {
    char line[LINE_MAX];
    FILE *file;

    if (chdir(pkg->path[0]) != 0) {
        die("Package '%s' not installed", pkg->name);
    }

    file = fopen("version", "r");

    if (!file) {
        die("Version file does not exist");
    }

    fgets(line, LINE_MAX, file) ;
    fclose(file);
}
