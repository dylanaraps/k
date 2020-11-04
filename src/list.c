#include <errno.h>
#include <stdlib.h>

#include "list.h"

int list_init(list *l, size_t s) {
    l->cap = s ? s : 16;
    l->len = 0;

    if (l->cap > (size_t)-1 / (sizeof(void *))) {
        return -ENOMEM;
    }

    l->arr = malloc(sizeof(void *) * l->cap);

    return l->arr ? 0 : -1;
}

static int list_grow(list *l) {
    size_t gr = l->cap + (l->cap >> 1);

    if (gr > (size_t)-1 / (sizeof(void *))) {
        return -ENOMEM;
    }

    void *n = realloc(l->arr, sizeof(void *) * gr);

    if (!n) {
        return -ENOMEM;
    }

    l->cap = gr;
    l->arr = n;

    return 0;
}

int list_push_b(list *l, void *d) {
    if (l->len == l->cap && list_grow(l) < 0) {
        return -ENOMEM;
    }

    l->arr[l->len++] = d;
    return 0;
}

void list_drop_b(list *l) {
    l->len -= (l->len ? 1 : 0);
}

void list_sort(list *l, int (*cb)(const void *, const void *)) {
    qsort(l->arr, l->len, sizeof(void *), cb);
}

void list_free(list *l) {
    free(l->arr);
}

