#ifndef KISS_REPO_H_
#define KISS_REPO_H_

char **repo_init(void);
void repo_free(char **);
char *repo_find(const char *, int, char **);

#endif
