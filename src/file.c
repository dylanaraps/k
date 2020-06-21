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

static char COPY_DEST[PATH_MAX];

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
    /* unused */
    (void)(sb);

    printf("Copying... %s\n", fp + fb->base);

    if (tf == FTW_D) {
        tf = mkdir(fp + fb->base, S_IRWXU);

        if (tf == -1 && errno != EEXIST) {
            die("Failed to create directory %s", fp + fb->base);
        }
    }

    if (tf == FTW_F) {
        copy_file(fp, fp + fb->base);
    }

    return 0;
}

void copy_dir(const char *src, const char *des) {
    int err;

    err = strlcpy(COPY_DEST, des, PATH_MAX);

    if (err > PATH_MAX) {
        die("strlcpy was truncated");
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

    if (err > PATH_MAX) {
        die("strlcpy truncated PATH");
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
