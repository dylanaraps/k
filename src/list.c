#include <stdio.h>  /* printf */
#include <unistd.h> /* chdir */

#include "log.h"
#include "list.h"

void pkg_list(package *pkg) {
    /* todo: prepend KISS_PATH */
    if (chdir("/var/db/kiss/installed") != 0) {
        die("Repository is not accessible");
    }

    if (chdir(pkg->name) != 0) {
        die("Package '%s' not installed", pkg->name);
    }

    printf("%s\n", pkg->name);
}
