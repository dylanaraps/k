#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>

#define die(...) _m(":(", __FILE__, __LINE__, __VA_ARGS__),exit(1)
#define msg(...) _m("OK", __FILE__, __LINE__, __VA_ARGS__)

static void _m(const char* t, const char *f, const int l, const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);

    printf("[%s] (%s:%d) ", t, f, l);
    vprintf(fmt, args);
    printf("\n");

    va_end(args);
}

static char *xstrdup(const char *s) {
    char *p = strdup(s);

    if (!p) {
        die("Failed to allocate memory");
    }

    return p;
}

static char *xgetenv(const char *s, char *f) {
    char *p = getenv(s);

    if (!p || !p[0]) {
        p = f;
    }

    return xstrdup(p);
}

static int xsnprintf(char *str, size_t size, const char *fmt, ...) {
    va_list va;
    int len;

    va_start(va, fmt);
    len = vsnprintf(str, size, fmt, va);
    va_end(va);

    if (len < 0 || (size_t)len >= size) {
        die("Failed to construct string");
    }

    return len;
}

static void cp_file(const char *src, const char *des, const int oct) {
    FILE *r;
    FILE *w;
    size_t err;
    char buf[4096]; /* todo: dynamically allocate buffer */

    r = fopen(src, "r");
    w = fopen(des, "w");

    if (!r) {
        die("Failed to read file");
    }

    if (!w) {
        die("Failed to write file %s", des);
    }

    for (;;) {
        err = fread(buf, 1, sizeof(buf), r);

        if (err != sizeof(buf) && ferror(r)) {
            die("File not accessible");
        }

        if (err == 0) {
            break;
        }

        if (fwrite(buf, 1, err, w) != err) {
            die("Cannot copy file");
        }
    }

    fclose(r);
    fclose(w);

    if (chmod(des, oct) == -1) {
        die("Failed to set permissions on %s", des);
    }
}

static void install_files(const char *src, const char *des, const char opt) {
    struct stat src_p;
    struct stat des_p;
    char *line = 0;
    char srcp[PATH_MAX];
    char desp[PATH_MAX];
    size_t line_l;
    int oct;

    while (getline(&line, &(size_t){0}, stdin) != -1) {
        line_l = strcspn(line, "\n");
        line[line_l] = 0;

        /* todo: dynamically reallocated buffer */
        xsnprintf(srcp, PATH_MAX, "%s%s", src, line);
        xsnprintf(desp, PATH_MAX, "%s%s", des, line);

        if (stat(srcp, &src_p) == -1) {
            die("Failed to copy '%s'\n", line);
        }

        if (stat(srcp, &des_p) == -1) {
            die("Failed to copy '%s'\n", line);
        }

        oct = src_p.st_mode & (S_IRUSR | S_IWUSR | S_IXUSR |
                               S_IRGRP | S_IWGRP | S_IXGRP |
                               S_IROTH | S_IWOTH | S_IXOTH);

        if (line[line_l - 1] == '/') {
            if (mkdir(desp, oct) == -1 && errno != EEXIST) {
                die("Failed to create directory %s\n", desp);
            }
        } else {
            if (S_ISDIR(des_p.st_mode) == 0) {
                if (opt == 's' && S_ISREG(des_p.st_mode) != 0) {
                    continue;
                }

                /* only add suid/sgid permissions to files */
                oct |= src_p.st_mode & (S_ISUID | S_ISGID);
                cp_file(srcp, desp, oct);
            }
        }
    }

    free(line);
}

int main(int argc, char *argv[]) {
    char *root = xgetenv("KISS_ROOT", "");

    if (!argv[1]) {
        die("Source directory must be provided");
    }

    install_files(argv[1], root, argv[2][0]);
    free(root);

    return 0;
}
