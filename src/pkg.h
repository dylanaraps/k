#ifndef KISS_PKG_H_
#define KISS_PKG_H_

#define DB_DIR "/var/db/kiss/installed"

typedef struct package {
    char *name;    
    char *path;
    char *ver;
} package;

package pkg_init(const char *);
void pkg_free(package *);
int pkg_list(const char *);
void pkg_list_all(package *);
void pkg_version(package *);

#endif
