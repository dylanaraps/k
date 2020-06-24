#define _POSIX_C_SOURCE 200809L
#include <stdio.h>    /* fork */
#include <limits.h>   /* PATH_MAX */
#include <unistd.h>   /* access, execvp */
#include <sys/wait.h> /* waitpid */

#include "extract.h"
#include "file.h"
#include "log.h"
#include "util.h"
#include "pkg.h"
#include "build.h"

static int exec_file(const char *file, char *args[]) {
    int err = fork();

    switch (err) {
    case -1:
        die("fork() failed");

    case 0:
        execvp(file, args);
        break;

    default:
        waitpid(err, &err, 0);
    }

    return err;
}

void pkg_build(package *pkg) {
    char build_file[PATH_MAX];
    int err = 0;

    /* extraction must occur this late */
    pkg_extract(pkg);

    xsnprintf(build_file, PATH_MAX, "%s/build", pkg->path);

    if (access(build_file, X_OK) == -1) {
        die("[%s] Build file not executable", pkg->name);
    }

    msg("[%s] Starting build", pkg->name);

    err = exec_file(build_file, (char*[]){
        build_file,
        pkg->pkg_dir,
        pkg->ver,
        NULL
    });

    if (err != 0) {
        die("[%s] Build failed", pkg->name);
    }

    msg("[%s] Successfully built package", pkg->name);

    xsnprintf(pkg->db_dir, PATH_MAX, "%s/%s/%s",
       pkg->pkg_dir,
       DB_DIR,
       pkg->name
    );

    /* copy repository files to /var/db/kiss/installed */
    cp_dir(pkg->path, pkg->db_dir);
}
