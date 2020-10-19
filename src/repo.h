#ifndef KISS_REPO_H_
#define KISS_REPO_H_

#include "str.h"

void repo_init(void);
void repo_find(str **buf, char *query);
void repo_find_all(str **s, char *query);
char *get_db_dir(void);
void repo_free(void);

#endif
