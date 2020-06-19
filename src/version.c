#define _POSIX_C_SOURCE 200809L
#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "version.h"
#include "log.h"
#include "util.h"
#include "pkg.h"

void pkg_version(package *pkg) {
    FILE *file;
    char *buf = NULL;
    char *tok;
    size_t len;

    SAVE_CWD;
    chdir(*pkg->path);

    file = xfopen("version", "r");

    getline(&buf, &(size_t){0}, file);

    if (!buf)
        die("version file is invalid");

    tok = strtok(buf,  " 	\r\n");
    len = strlen(tok) + 1;

    if (!tok)
        die("Invalid version file");

    pkg->ver = xmalloc(len);
    strlcpy(pkg->ver, tok, len);

    tok = strtok(NULL, " 	\r\n");
    len = strlen(tok) + 1;

    if (!tok)
        die("release field empty");

    pkg->rel = xmalloc(len);
    strlcpy(pkg->rel, tok, len);

    free(buf);
    free(tok);
    fclose(file);

    LOAD_CWD;
}
