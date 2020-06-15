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
        tok = strtok(NULL, del);
    }

    res = realloc (res, sizeof(char*) * (n + 1));
    res[n] = 0;

    return res;
}

char *join_string(char *str, char *str2, char *del) {
    int buf_size = strlen(str) + strlen(str2) + strlen(del) + 1;
    char *buf = malloc(buf_size);

    if (!buf) {
        printf("error: Failed to allocate memory\n");
        exit(1);
    }

    if (snprintf(buf, buf_size, "%s%s%s", str, del, str2) >= buf_size) {
        printf("error: Not enough space for allocation\n");
        exit(1);
    }

    return buf;
}
