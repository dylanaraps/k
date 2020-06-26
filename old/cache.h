#include <stdlib.h> /* size_t */
#include <limits.h> /* PATH_MAX */

#include "pkg.h"

extern char CAC_DIR[PATH_MAX];

void cache_init(void);
void cache_destroy(void);
void state_dir_init(package *pkg, const char *type, char *buf);
void cache_dir_init(package *pkg, const char *type, char *buf);
