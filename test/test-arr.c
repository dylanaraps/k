#include <stdio.h>
#include <string.h>

#include "arr.h"
#include "test.h"
#include "util.h"

static const char *fruit[] = {
    "apple",
    "orange",
    "pear",
    "banana",
    "peach",
    "lime"
};

int main(int argc, char *argv[]) {
    (void) argc;
    (void) argv;

    test_begin(__FILE__);

    char **list = arr_alloc(0, 4); {
        test(list);
        test(arr_cap(list) == 4);
        test(arr_len(list) == 0);
    }

    for (size_t i = 0; i < sizeof(fruit) / sizeof(fruit[0]); i++) {
        char *s = strdup(fruit[i]);

        arr_push_b(list, s); {
            test(arr_len(list) == i + 1);

            for (size_t i = 0; i < arr_len(list); i++) {
                test(strcmp(list[i], fruit[i]) == 0);
            }
        }
    }

    arr_sort(list, qsort_cb_str); {
        test(arr_cap(list) == 6);
        test(arr_len(list) == 6);
        test(strcmp(list[0], "apple") == 0);
        test(strcmp(list[1], "banana") == 0);
        test(strcmp(list[2], "lime") == 0);
        test(strcmp(list[3], "orange") == 0);
        test(strcmp(list[4], "peach") == 0);
        test(strcmp(list[5], "pear") == 0);
    }

    free(list[arr_len(list) - 1]);
    arr_drop_b(list); {
        test(arr_cap(list) == 6);
        test(arr_len(list) == 5);
        test(strcmp(list[0], "apple") == 0);
        test(strcmp(list[1], "banana") == 0);
        test(strcmp(list[2], "lime") == 0);
        test(strcmp(list[3], "orange") == 0);
        test(strcmp(list[4], "peach") == 0);
    }

    free(list[arr_len(list) - 1]);
    arr_drop_b(list); {
        test(arr_cap(list) == 6);
        test(arr_len(list) == 4);
        test(strcmp(list[0], "apple") == 0);
        test(strcmp(list[1], "banana") == 0);
        test(strcmp(list[2], "lime") == 0);
        test(strcmp(list[3], "orange") == 0);
    }

    for (size_t i = 0; i < arr_len(list); i++) {
        free(list[i]);
    }
    arr_free(list);

    return test_finish();
}

