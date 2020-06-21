#include <stdlib.h> /* size_t */
#include <limits.h> /* PATH_MAX */

#include "pkg.h"

extern const char *caches[];
extern const char *states[];

extern char CAC_DIR[PATH_MAX];

void cache_init(void);
void cache_destroy(int s);
void xdg_cache_dir(char *buf, int len);
void state_init(package *pkg, const char *type, char *buf);
