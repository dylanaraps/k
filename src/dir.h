#ifndef KISS_DIR_H_
#define KISS_DIR_H_

#include <sys/types.h>

int mkdir_p(char *, const mode_t);
int rm_rf(const char *);
int is_dir(const char *);

#endif
