#define _POSIX_C_SOURCE 200809L
#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "version.h"
#include "log.h"
#include "pkg.h"

void pkg_version(package *pkg) {
    FILE *file;
    char *buf = 0;
    char *tok;

    SAVE_CWD;
    chdir(*pkg->path);

    file = fopen("version", "r");

    if (!file)
        log_error("version file does not exist");

    getline(&buf, &(size_t){0}, file);

    if (!buf)
        log_error("version file is invalid");

    tok = strtok(buf,  " 	\r\n");

    if (!tok)
        log_error("Invalid version file");

    pkg->ver = xmalloc(strlen(tok) + 1);
    strlcpy(pkg->ver, tok, sizeof(tok));

    tok = strtok(NULL, " 	\r\n");

    if (!tok)
        log_error("release field empty");

    pkg->rel = xmalloc(strlen(tok) + 1);
    strlcpy(pkg->rel, tok, sizeof(tok));

    fclose(file);
    LOAD_CWD;
}
