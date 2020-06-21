#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <limits.h>   /* PATH_MAX */
#include <unistd.h>   /* access */
#include <sys/stat.h> /* mkdir */
#include <ftw.h>      /* ntfw */
#include <errno.h>    /* errno */
#include <libgen.h>   /* dirname */

#include "log.h"
#include "strl.h"
#include "file.h"

static char COPY_DES[PATH_MAX];
static char COPY_SRC[PATH_MAX];
static char COPY_END[PATH_MAX];

void copy_file(const char *src, const char *des) {
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

static int copy(const char *fp, const struct stat *sb, int tf, struct FTW *fb) {
    int err;

    /* unused */
    (void)(sb);
    (void)(fb);

    err = snprintf(COPY_END, PATH_MAX, "%s/%s",
                   COPY_DES, fp + strlen(COPY_SRC));

    if (err >= PATH_MAX) {
        die("strlcpy failed");
    }

    if (tf == FTW_D) {
        mkdir_p(COPY_END);
    }

    if (tf == FTW_F) {
        copy_file(fp, COPY_END);
    }

    return 0;
}

void copy_dir(const char *src, const char *des) {
    int err;

    err = strlcpy(COPY_SRC, src, PATH_MAX);

    if (err >= PATH_MAX) {
        die("strlcpy failed");
    }

    err = strlcpy(COPY_DES, des, PATH_MAX);

    if (err >= PATH_MAX) {
        die("strlcpy failed");
    }

    nftw(src, copy, 256, 0);
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

    err = strlcpy(tmp, dir, PATH_MAX);

    if (err >= PATH_MAX) {
        die("strlcpy failed");
    }

    for (p = tmp + 1; *p; p++) {
       if (*p == '/') {
           *p = 0;

           err = mkdir(tmp, S_IRWXU);

           if (err == -1 && errno != EEXIST) {
               die("Failed to create directory %s", tmp);
           }

           *p = '/';
       }
    }

    err = mkdir(tmp, S_IRWXU);

    if (err == -1 && errno != EEXIST) {
        die("Failed to create directory %s", tmp);
    }
}
