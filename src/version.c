#define _POSIX_C_SOURCE 200809L
#include <stdio.h>  /* FILE */
#include <unistd.h> /* chdir */
#include <limits.h> /* LINE_MAX */
#include <string.h> /* strchr */

#include "log.h"
#include "strl.h"
#include "util.h"
#include "version.h"

void pkg_version(package *pkg) {
    char *line = 0;
    FILE *file;
    char *tok;
    size_t len;
    size_t err;

    if (chdir(pkg->path[0]) != 0) {
        die("Package '%s' not installed", pkg->name);
    }

    file = fopen("version", "r");

    if (!file) {
        die("Version file does not exist");
    }

    err = getline(&line, &(size_t){0}, file);

    if ((int) err == -1) {
        die("Failed to read version file");
    }

    tok = strtok(line, " 	");

    if (!tok) {
        die("Invalid version file");
    }

    len = strlen(tok) + 1;
    pkg->ver = xmalloc(len);

    err = strlcpy(pkg->ver, tok, len);

    if (err > len) {
        die("strlcpy was truncated");
    }

    tok = strtok(NULL, " 	\r\n");

    if (!tok) {
        die("Release field missing");
    }

    len = strlen(tok) + 1;
    pkg->rel = xmalloc(len);

    err = strlcpy(pkg->rel, tok, len);

    if (err > len) {
        die("strlcpy was truncated");
    }

    free(line);
    fclose(file);
}
