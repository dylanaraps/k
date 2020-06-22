extern char **REPOS;
extern int REPO_LEN;

void repo_init(void);

#ifdef FREE_ON_EXIT
void repo_destroy(void);
#endif
