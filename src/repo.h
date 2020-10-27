#ifndef KISS_REPO_H
#define KISS_REPO_H

int repo_init(void);
int repo_find(char **buf, const char *name);
void repo_free(void);

#endif
