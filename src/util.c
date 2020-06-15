#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

char **split_string(char *str, char *del) {
    char *tok;
    char **res = NULL;
    int  n = 0;

    if (!str || str[0] == '\0') {
        return NULL;
    }

    tok = strtok(str, del); 

    while (tok) {
        res = realloc(res, sizeof(char*) * ++n);
        
        if (res == NULL) {
            printf("Failed to allocate memory\n");
            exit(1);
        }

        res[n - 1] = tok;
        tok = strtok(NULL, ":");
    }

    res = realloc (res, sizeof(char*) * (n + 1));
    res[n] = 0;

    return res;
}
