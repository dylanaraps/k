#include <string.h>

#include "list.h"
#include "test.h"

static const char *fruit[] = {
    "apple",
    "orange",
    "pear",
    "banana",
    "peach",
    "lime"
};

static int compare(void const *a, void const *b) {
    return strcmp(*(const char **) a, *(const char **) b);
}

int main(int argc, char *argv[]) {
    (void) argc;
    (void) argv;

    int ret = test_begin(__FILE__);

    list l;
    ret = list_init(&l, 0); {
        test(ret == 0);
        test(l.cap == 16);
        test(l.len == 0);
    }

    for (size_t i = 0; i < sizeof(fruit) / sizeof(fruit[0]); i++) {
        char *s = strdup(fruit[i]);

        ret = list_push_b(&l, s); {
            test(ret == 0);
            test(l.cap == 16);
            test(l.len == i + 1);
            test(strcmp(l.arr[0], "apple") == 0);
        }
    }

    list_sort(&l, compare); {
        test(l.cap == 16);
        test(l.len == 6);
        test(strcmp(l.arr[0], "apple") == 0);
        test(strcmp(l.arr[1], "banana") == 0);
        test(strcmp(l.arr[2], "lime") == 0);
        test(strcmp(l.arr[3], "orange") == 0);
        test(strcmp(l.arr[4], "peach") == 0);
        test(strcmp(l.arr[5], "pear") == 0);
    }

    list_drop_b(&l); {
        test(l.cap == 16);
        test(l.len == 5);
        test(strcmp(l.arr[0], "apple") == 0);
        test(strcmp(l.arr[1], "banana") == 0);
        test(strcmp(l.arr[2], "lime") == 0);
        test(strcmp(l.arr[3], "orange") == 0);
        test(strcmp(l.arr[4], "peach") == 0);
        test(strcmp(l.arr[5], "pear") == 0);
    }

    list_drop_b(&l); {
        test(l.cap == 16);
        test(l.len == 4);
        test(strcmp(l.arr[0], "apple") == 0);
        test(strcmp(l.arr[1], "banana") == 0);
        test(strcmp(l.arr[2], "lime") == 0);
        test(strcmp(l.arr[3], "orange") == 0);
        test(strcmp(l.arr[4], "peach") == 0);
        test(strcmp(l.arr[5], "pear") == 0);
    }

    for (size_t i = 0; i < l.len; i++) {
        free(l.arr[i]);
    }
    list_free(&l);

    return test_finish();
}

