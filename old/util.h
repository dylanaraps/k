#include <sys/stat.h>

void die(char *type, char *message);
char *strjoin(char *str, const char *str2, const char *delim);
char **str_to_array(char *str, char *del);
void cache_init(void);
