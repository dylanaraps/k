#ifndef KISS_PKG_H_
#define KISS_PKG_H_

void pkg_version(str **s, const char *name, const char *repo);
void pkg_list_print(str **s, const char *name, const char *repo);
void pkg_list_installed(str **s, const char *repo);

#endif
