#ifndef KISS_REPO_H_
#define KISS_REPO_H_

#include "str.h"

void repo_init(void);
void repo_find(str **s, const char *query);
void repo_find_all(str **s, const char *query);
char *get_db_dir(void);
char *get_kiss_path(void);
void repo_free(void);

#endif
