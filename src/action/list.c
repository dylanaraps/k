#include <errno.h>
#include <stdio.h>

#include "error.h"
#include "str.h"
#include "action.h"

static int pkg_list(str **buf, const char *pkg) {
    str_push_l(buf, "/var/db/kiss/installed/", 23);
    str_push_s(buf, pkg);
    str_push_l(buf, "/version", 8);

    FILE *ver = fopen((*buf)->buf, "r");

    if (!ver) {
        if (errno == ENOENT) {
            err("package '%s' not installed", pkg);

        } else {
            err_no("failed to open file '%s'", (*buf)->buf);
        }

        return -1;
    }

    str_set_len(*buf, 0);
    int ret = 0;

    if ((ret = str_getline(buf, ver)) == 0) {
        printf("%s %s\n", pkg, (*buf)->buf);

    } else {
        err_no("file read '...%s/version' failed", pkg);
    }

    fclose(ver);
    return ret;
}

int action_list(int argc, char *argv[]) {
    str *buf = str_init(1024);

    if (!buf) {
        err("failed to allocate memory");
        return -ENOMEM;
    }

    for (int i = 2; i < argc; i++) {
        if (pkg_list(&buf, argv[i]) < 0) {
            str_free(&buf);
            return -1;
        }

        // soft reset buffer
        str_set_len(buf, 0);
    }

    if (argc == 2) {
        // pkg_list_all
    }

    str_free(&buf);
    return 0;
}

