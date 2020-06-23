#define _POSIX_C_SOURCE 200809L
#include <stdio.h>  /* FILE */
#include <limits.h> /* LINE_MAX */
#include <string.h> /* strchr */
#include <fcntl.h>  /* O_RDONLY */

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

    file = fopenat(pkg->path, "version", "r");

    if (!file) {
        die("[%s] Version file does not exist", pkg->name);
    }

    err = getline(&line, &(size_t){0}, file);

    if ((int) err == -1) {
        die("[%s] Failed to read version file", pkg->name);
    }

    tok = strtok(line, " 	");

    if (!tok) {
        die("[%s] Invalid version file", pkg->name);
    }

    len = strlen(tok) + 1;
    pkg->ver = xmalloc(len);

    err = strlcpy(pkg->ver, tok, len);

    if (err >= len) {
        die("strlcpy failed");
    }

    tok = strtok(NULL, " 	\r\n");

    if (!tok) {
        die("[%s] Release field missing", pkg->name);
    }

    len = strlen(tok) + 1;
    pkg->rel = xmalloc(len);

    err = strlcpy(pkg->rel, tok, len);

    if (err >= len) {
        die("strlcpy failed");
    }

    free(line);
    fclose(file);
}
