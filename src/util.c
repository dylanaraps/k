#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "mkdir.h"
#include "util.h"
#include "kiss.h"

char *strjoin(char *str, char *str2, char *delim) {
    char *join = malloc(strlen(str) + strlen(str2) + strlen(delim) + 1);

    strcpy(join, str);
    strcat(join, delim);
    strcat(join, str2);

    return join;
}

void cache_init(void) {
    mode_t mode, dir_mode; 

    pid_t pid = getpid();
    HOME      = getenv("HOME");
    CAC_DIR   = getenv("XDG_CACHE_HOME");

    if (!HOME || HOME[0] == '\0') {
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

    /*
     * The default file mode is a=rwx (0777) with selected permissions
     * removed in accordance with the file mode creation mask.  For
     * intermediate path name components, the mode is the default modified
     * by u+wx so that the subdirectories can always be created.
     */
	mode     = 0777 & ~umask(0);
	dir_mode = mode | S_IWUSR | S_IXUSR;

    if (mkpath(MAK_DIR, mode, dir_mode) != 0)
        exit(1);

    if (mkpath(PKG_DIR, mode, dir_mode) != 0)
        exit(1);

    if (mkpath(TAR_DIR, mode, dir_mode) != 0)
        exit(1);

    if (mkpath(SRC_DIR, mode, dir_mode) != 0)
        exit(1);

    if (mkpath(LOG_DIR, mode, dir_mode) != 0)
        exit(1);

    if (mkpath(BIN_DIR, mode, dir_mode) != 0)
        exit(1);
}
