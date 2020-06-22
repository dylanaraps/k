#define _POSIX_C_SOURCE 200809L
#include <stdio.h>  /* fopen, FILE */
#include <unistd.h> /* chdir */

#include "log.h"
#include "util.h"
#include "pkg.h"
#include "depends.h"

void pkg_depends(package *pkg) {
    package *tmp;
    FILE *file;
    int  len;

    if (chdir(pkg->path[0]) != 0) {
        die("[%s] Repository is not accessible (%s)", pkg->name, pkg->path[0]);
    }

    file = fopen("depends", "r");

    if (!file) {
        die("[%s] Failed to open sources file", pkg->name);
    }

    len = cntlines(file);

    if (len == 0) {
        return;
    }
}

void resolve_dep(package *pkg) {

}
/* pkg_depends() { */
/*     # Resolve all dependencies and generate an ordered list. This does a */
/*     # depth-first search. The deepest dependencies are listed first and then */
/*     # the parents in reverse order. */
/*     contains "$deps" "$1" || { */
/*         # Filter out non-explicit, aleady installed dependencies. */
/*         [ "$3" ] && [ -z "$2" ] && (pkg_list "$1" >/dev/null) && return */

/*         # Recurse through the dependencies of the child packages. */
/*         while read -r dep _ || [ "$dep" ]; do */
/*             [ "${dep##\#*}" ] && pkg_depends "$dep" '' "$3" */
/*         done 2>/dev/null < "$(pkg_find "$1")/depends" ||: */

/*         # After child dependencies are added to the list, */
/*         # add the package which depends on them. */
/*         [ "$2" = explicit ] || deps="$deps $1 " */
/*     } */
/* } */
