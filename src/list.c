#define _POSIX_C_SOURCE 200809L
#include <stdio.h>  /* printf */
#include <unistd.h> /* chdir */
#include <dirent.h> /* scandir */
#include <fcntl.h>  /* open */

#include "log.h"
#include "pkg.h"
#include "list.h"

int pkg_list(package *pkg) {
    int fd;
    int err;

    fd = open("/var/db/kiss/installed", O_RDONLY | O_DIRECTORY);

    if (fd == -1) {
        die("[%s] Package DB not accessible", pkg->name);
    }

    err = openat(fd, pkg->name, O_RDONLY | O_DIRECTORY);
    close(fd);

    if (err == -1) {
        return 1;
    }

    close(err);
    return 0;
}

void pkg_list_all(package *pkg) {
    package *tmp = pkg;
    struct dirent  **list;
    int err;
    int i;

    if (!pkg) {
        err = scandir("/var/db/kiss/installed", &list, NULL, alphasort);

        if (err == -1) {
            die("Package DB not accessible");
        }

        // '2' skips '.'/'..'.
        for (i = 2; i < err; i++) {
            pkg_init(&pkg, list[i]->d_name);
            free(list[i]);
        }

        free(list);
    }

    for (tmp = pkg; tmp; tmp = tmp->next) {
        err = pkg_list(tmp);

        if (err == 0) {
            printf("%s %s %s\n", tmp->name, tmp->ver, tmp->rel);
        } else {
            die("[%s] Package not installed", tmp->name);
        }
    }
}
