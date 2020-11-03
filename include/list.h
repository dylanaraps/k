#ifndef KISS_LIST_H
#define KISS_LIST_H

typedef struct {
	void **arr;
	size_t cap;
	size_t len;
} list;

int list_init(list *l);
int list_grow(list *l);
int list_push_b(list *l, void *d);
void list_drop_b(list *l);
void list_sort(list *l, int (*cb)(const void *, const void *));
void list_free(list *l);

#endif
