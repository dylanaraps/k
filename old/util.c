#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "mkdir.h"
#include "util.h"
#include "kiss.h"

char *strjoin(char *str, const char *str2, const char *delim) {
    char *join = malloc(strlen(str) + strlen(str2) + strlen(delim) + 1);

    if (!join) {
        return 0;
    }

    strcpy(join, str);
    strcat(join, delim);
    strcat(join, str2);

    return join;
}

char **str_to_array(char *str, char *del) {
    char *tok;
    char **res = NULL;
    int  n = 0;

    if (!str || str[0] == '\0') {
        return NULL;
    }

    tok = strtok(str, del); 

    while (tok) {
        res = realloc(res, sizeof(char*) * ++n);
        
        if (res == NULL) {
            printf("Failed to allocate memory\n");
            exit(1);
        }

        res[n - 1] = tok;
        tok = strtok(NULL, ":");
    }

    res = realloc (res, sizeof(char*) * (n + 1));
    res[n] = 0;

    return res;
}

void cache_init(void) {
    HOME      = getenv("HOME");
    CAC_DIR   = getenv("XDG_CACHE_HOME");

    if (!HOME || HOME[0] == '\0') {
        printf("HOME directory is NULL\n");
        exit(1);
    }

    if (!CAC_DIR || CAC_DIR[0] == '\0') {
        CAC_DIR = strjoin(HOME, ".cache", "/");
    }

    CAC_DIR = strjoin(CAC_DIR,    "kiss", "/");
    MAK_DIR = strjoin(CAC_DIR,   "build", "/");
    PKG_DIR = strjoin(CAC_DIR,     "pkg", "/");
    TAR_DIR = strjoin(CAC_DIR, "extract", "/");
    SRC_DIR = strjoin(CAC_DIR, "sources", "/");
    LOG_DIR = strjoin(CAC_DIR,    "logs", "/");
    BIN_DIR = strjoin(CAC_DIR,     "bin", "/");

    if (mkpath(MAK_DIR) != 0) {
        printf("Failed to create %s\n", MAK_DIR);
        exit(1);
    }

    if (mkpath(PKG_DIR) != 0) {
        printf("Failed to create %s\n", PKG_DIR);
        exit(1);
    }

    if (mkpath(TAR_DIR) != 0) {
        printf("Failed to create %s\n", TAR_DIR);
        exit(1);
    }

    if (mkpath(SRC_DIR) != 0) {
        printf("Failed to create %s\n", SRC_DIR);
        exit(1);
    }

    if (mkpath(LOG_DIR) != 0) {
        printf("Failed to create %s\n", LOG_DIR);
        exit(1);
    }

    if (mkpath(BIN_DIR) != 0) {
        printf("Failed to create %s\n", BIN_DIR);
        exit(1);
    }
}
