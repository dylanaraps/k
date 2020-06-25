#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700 /* nftw */
#include <stdio.h>
#include <limits.h>   /* PATH_MAX */
#include <unistd.h>   /* access */
#include <sys/stat.h> /* mkdir */
#include <ftw.h>      /* ntfw */
#include <string.h>   /* strlen */
#include <errno.h>    /* errno */
#include <libgen.h>   /* dirname */

#include "log.h"
#include "util.h"
#include "file.h"

static char COPY_DES[PATH_MAX];
static char COPY_SRC[PATH_MAX];
static char COPY_END[PATH_MAX];

static int cp(const char *fp, const struct stat *sb, int tf, struct FTW *fb) {
    (void)(sb);
    (void)(fb);

    xsnprintf(COPY_END, PATH_MAX, "%s/%s", COPY_DES, fp + strlen(COPY_SRC));

    if (tf == FTW_D) {
        mkdir_p(COPY_END);
    }

    if (tf == FTW_F) {
        cp_file(fp, COPY_END);
    }

    return 0;
}

static int rm(const char *fp, const struct stat *sb, int tf, struct FTW *fb) {
    int rv;

    // inused
    (void)(sb);
    (void)(tf);
    (void)(fb);

    rv = remove(fp);

    if (rv) {
        msg("warning: Failed to remove %s", fp);
    }

    return rv;
}

void rm_dir(const char *src) {
    /* todo: magic 64 */
    nftw(src, rm, 64, FTW_DEPTH | FTW_PHYS);
}

void cp_dir(const char *src, const char *des) {
    xmemcpy(COPY_SRC, src, PATH_MAX);
    xmemcpy(COPY_DES, des, PATH_MAX);

    /* todo: magic 64 */
    nftw(src, cp, 64, 0);
}

void cp_file(const char *src, const char *des) {
    FILE *r;
    FILE *w;
    int err;
    char buf[BUFSIZ];

    r = fopen(src, "rb");

    if (!r) {
        die("Failed to read file");
    }

    w = fopen(des, "wb");

    if (!w) {
        die("Failed to read file");
    }

    for (;;) {
        err = fread(buf, 1, sizeof(BUFSIZ), r);

        if (err == -1) {
            die("File not accessible");
        }

        if (err == 0) {
            break;
        }

        err = fwrite(buf, 1, err, w);

        if (err == -1) {
            die("Cannot copy file");
        }
    }

    fclose(r);
    fclose(w);
}

void mkdir_p(const char *dir) {
    char tmp[PATH_MAX];
    int err;
    char *p = 0;

    if (!dir) {
        die("mkdir input empty");
    }

    if (access(dir, F_OK) != -1) {
        return;
    }

    xmemcpy(tmp, dir, PATH_MAX);

    for (p = tmp + 1; *p; p++) {
       if (*p == '/') {
           *p = 0;

           err = mkdir(tmp, 0755);

           if (err == -1 && errno != EEXIST) {
               die("Failed to create directory %s", tmp);
           }

           *p = '/';
       }
    }

    err = mkdir(tmp, 0755);

    if (err == -1 && errno != EEXIST) {
        die("Failed to create directory %s", tmp);
    }
}
