#include <errno.h>
#include <stdlib.h>

#include "list.h"

// overallocation amount
#define PAD 16

int list_init(list *l) {
    l->cap = PAD;
    l->len = 0;
    l->arr = malloc(sizeof(void *) * l->cap);

    return l->arr ? 0 : -1;
}

int list_grow(list *l) {
    if ((l->cap + PAD) > (size_t)-1 / (sizeof(void *))) {
        return -ENOMEM;
    }

    void *n = realloc(l->arr, sizeof(void *) * (l->cap + PAD));

    if (!n) {
        return -ENOMEM;
    }

    l->cap += PAD;
    l->arr = n;

    return 0;
}

int list_push_b(list *l, void *d) {
    if (l->len == l->cap && list_grow(l) < 0) {
        return -1;
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

