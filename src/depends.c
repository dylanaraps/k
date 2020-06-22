#define _POSIX_C_SOURCE 200809L
#include <stdio.h>  /* fopen, FILE */
#include <unistd.h> /* chdir, access */
#include <string.h> /* strtok */

#include "log.h"
#include "list.h"
#include "util.h"
#include "strl.h"
#include "pkg.h"
#include "depends.h"

void pkg_depends(package *pkg) {
    char *line = 0;
    char *tok;
    FILE *file;
    int i = 0;
    size_t ret;

    file = fopenat(pkg->path, "depends", "r");

    if (!file) {
        die("[%s] Failed to open depends file", pkg->name);
    }

    pkg->dep_l = cntlines(file);

    if (pkg->dep_l == 0) {
        return;
    }

    pkg->dep      = xmalloc((pkg->dep_l + 1) * sizeof(char *));
    pkg->dep_type = xmalloc((pkg->dep_l + 1) * sizeof(char *));

    while (getline(&line, &(size_t){0}, file) != -1) {
        if (line[0] == '#' || line[0] == '\n') {
            continue;
        }

        if (i > pkg->dep_l) {
            die("[%s] Mismatch in depends parser", pkg->name);
        }

        tok = strtok(line, " 	\r\n");

        if (!tok) {
            die("[%s] Invalid depends file", pkg->name);
        }

        ret = strlen(tok) + 1;
        pkg->dep[i] = xmalloc(ret);

        ret = strlcpy(pkg->dep[i], tok, ret);

        if (ret >= PATH_MAX) {
            die("strlcpy failed");
        }

        tok = strtok(NULL, " 	\r\n");
        tok = tok ? tok : "";

        ret = strlen(tok) + 1;
        pkg->dep_type[i] = xmalloc(ret);

        ret = strlcpy(pkg->dep_type[i], tok, ret);

        if (ret >= PATH_MAX) {
            die("strlcpy failed");
        }

        i++;
    }

    free(line);
}

void resolve_dep(package *pkg) {
    if (pkg_have(pkg->name) != 0) {
        if (pkg_list(pkg) == 0) {
            return;
        }
    }
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
