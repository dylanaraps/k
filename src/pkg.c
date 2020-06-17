#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <libgen.h>
#include <stdint.h>
#include <sys/stat.h>

#include <sys/stat.h>
#include <ftw.h>

#include "find.h"
#include "log.h"
#include "pkg.h"

void pkg_load(package **head, char *pkg_name) {
    package *new_pkg = (package*) malloc(sizeof(package));    
    package *last = *head;

    if (!new_pkg) {
        log_error("Failed to allocate memory");
    }

    new_pkg->next        = NULL;
    new_pkg->name        = pkg_name;
    new_pkg->sums        = 0;
    new_pkg->path        = 0;
    new_pkg->source.src  = 0;
    new_pkg->source.dest = 0;
    new_pkg->src_len     = 0;
    new_pkg->path_len    = 0;

    pkg_find(new_pkg);

    if (!*head) {
        new_pkg->prev = NULL;
        *head = new_pkg;
        return;
    }

    while (last->next) {
        last = last->next;
    }

    last->next = new_pkg;
    new_pkg->prev = last;
}

void cache_init(void) {
    HOME      = getenv("HOME");
    CAC_DIR   = getenv("XDG_CACHE_HOME");
    char cwd[PATH_MAX];
    char build_dir[PATH_MAX];
    char extract_dir[PATH_MAX];
    char pkg_dir[PATH_MAX];
    pid_t pid = getpid();

    if (!HOME || HOME[0] == '\0') {
        log_error("HOME is NULL");
    }

    if (!CAC_DIR || CAC_DIR[0] == '\0') {
        SAVE_CWD(HOME);

        mkdir(".cache", 0777);

        if (chdir(".cache") != 0) {
            goto err;
        }
        
        CAC_DIR = strdup(getcwd(cwd, sizeof(cwd)));
    }

    mkdir(CAC_DIR, 0777);

    if (chdir(CAC_DIR) != 0) {
        goto err;
    }

    mkdir("kiss", 0777);

    if (chdir("kiss") != 0) {
        goto err;
    }

    CAC_DIR = strdup(getcwd(cwd, sizeof(cwd)));

    sprintf(build_dir,   "%s/build-%jd",   CAC_DIR, (intmax_t) pid);
    sprintf(pkg_dir,     "%s/pkg-%jd",     CAC_DIR, (intmax_t) pid);
    sprintf(extract_dir, "%s/extract-%jd", CAC_DIR, (intmax_t) pid);

    mkdir(build_dir,   0777);
    mkdir(pkg_dir,     0777);
    mkdir(extract_dir, 0777);
    mkdir("sources",   0777);
    mkdir("logs",      0777);  

    if (chdir(build_dir) != 0) {
        goto err;
    }
    MAK_DIR = strdup(getcwd(cwd, sizeof(cwd)));

    if (chdir(pkg_dir) != 0) {
        goto err;
    }
    PKG_DIR = strdup(getcwd(cwd, sizeof(cwd)));

    if (chdir(extract_dir) != 0) {
        goto err;
    }
    TAR_DIR = strdup(getcwd(cwd, sizeof(cwd)));

    if (chdir("../sources") != 0) {
        goto err;
    }
    SRC_DIR = strdup(getcwd(cwd, sizeof(cwd)));

    if (chdir("../logs") != 0) {
        goto err;
    }
    LOG_DIR = strdup(getcwd(cwd, sizeof(cwd)));

    if (chdir("../bin") != 0) {
        goto err;
    }
    BIN_DIR = strdup(getcwd(cwd, sizeof(cwd)));

    LOAD_CWD;
    return;

err:
    log_error("Failed to create cache directory");
}

static int rm(const char *fpath, const struct stat *sb, int tf, struct FTW *fb) {
    // Unused.
    (void)(sb);
    (void)(tf);
    (void)(fb);

    int rv = remove(fpath);

    if (rv) {
        log_warn("Failed to remove %s", fpath);
    }

    return rv;
}

void cache_destroy(void) {
    nftw(MAK_DIR, rm, 64, FTW_DEPTH | FTW_PHYS);
    nftw(PKG_DIR, rm, 64, FTW_DEPTH | FTW_PHYS);
    nftw(TAR_DIR, rm, 64, FTW_DEPTH | FTW_PHYS);
}
