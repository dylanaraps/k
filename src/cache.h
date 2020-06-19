#include <stdlib.h> /* size_t */
#include <limits.h> /* PATH_MAX */

extern const char *caches[];
extern const char *states[];

extern char CAC_DIR[PATH_MAX];

void cache_init(void);
void cache_destroy(void);
void xdg_cache_dir(char *buf, size_t len);
