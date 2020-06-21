#define _POSIX_C_SOURCE 200809L
#include <stdio.h>    /* snprintf */
#include <limits.h>   /* PATH_MAX */
#include <unistd.h>   /* access */
#include <sys/wait.h> /* waitpid */

#include "extract.h"
#include "log.h"
#include "pkg.h"
#include "build.h"

void pkg_build(package *pkg) {
    char build_file[PATH_MAX];
    int err = 0;

    /* extraction must occur this late */
    pkg_extract(pkg);

    err = snprintf(build_file, PATH_MAX, "%s/build", pkg->path[0]);

    if (err < 1) {
        die("[%s] Failed to find build file", pkg->name);
    }

    if (err > PATH_MAX) {
        die("[%s] Build file exceeds PATH_MAX", pkg->name);
    }

    if (access(build_file, X_OK) == -1) {
        die("[%s] Build file not executable", pkg->name);
    }

    msg("[%s] Starting build", pkg->name);

    switch (err = fork()) {
    case -1:
        die("fork() failed");

    case 0:
        execvp(build_file, (char*[]){
            build_file,
            pkg->pkg_dir,
            pkg->ver,
            NULL
        });
        break;

    default:
        waitpid(err, NULL, 0);
    }
}
