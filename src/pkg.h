#ifndef KISS_PKG_H_
#define KISS_PKG_H_

#define DB_DIR "/var/db/kiss/installed"

typedef struct package {
    char *name;    
    char *path;
} package;

package pkg_init(const char *);
void pkg_free(package *);
package *pkg_init_db(void);
int pkg_list(const char *);

#endif
