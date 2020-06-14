#include <stdlib.h>
#include <string.h>

#include "util.h"

char *strjoin(char *str, char *str2, char *delim) {
    char *join = malloc(strlen(str) + strlen(str2) + strlen(delim) + 1);

    strcpy(join, str);
    strcat(join, delim);
    strcat(join, str2);

    return join;
}
