#ifndef KISS_PKG_H_
#define KISS_PKG_H_

#define DB_DIR "/var/db/kiss/installed"

typedef struct package {
    char *name;    
    char *path;
} package;

package pkg_init(const char *);
void pkg_free(package *);
int pkg_list(const char *);
void pkg_list_all(package *);
char *pkg_version(const char *, const char *);

#endif
