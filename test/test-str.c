#include <errno.h>
#include <string.h>

#include "str.h"
#include "test.h"

int main(int argc, char *argv[]) {
    (void) argc;
    (void) argv;

    int ret = test_begin(__FILE__);

    str *s = str_alloc(0, 0); {
        test(s);
        test(str_len(s) == 0);
        test(str_cap(s) == 0);
    }

    ret = str_push_s(&s, "hello, world"); {
        test(ret == 0);
        test(str_len(s) == 12);
        test(str_cap(s) == 18);
        test(strcmp(s, "hello, world") == 0);
    }

    str_set_len(s, 0); {
        test(str_len(s) == 0);
        test(str_cap(s) == 18);
        test(s[0] == 0);
        test(strcmp(s, "") == 0);
    }

    ret = str_push_l(&s, "123456", 6); {
        test(ret == 0);
        test(str_len(s) == 6);
        test(str_cap(s) == 18);
        test(strcmp(s, "123456") == 0);
    }

    ret = str_push_c(&s, '!'); {
        test(ret == 0);
        test(str_len(s) == 7);
        test(str_cap(s) == 18);
        test(strcmp(s, "123456!") == 0);
    }

    str_rstrip(&s, '!'); {
        test(str_len(s) == 6);
        test(str_cap(s) == 18);
        test(strcmp(s, "123456") == 0);
    }

    ret = str_undo_c(&s, '6'); {
        test(ret == 0);
        test(str_len(s) == 5);
        test(str_cap(s) == 18);
        test(strcmp(s, "12345") == 0);
    }

    ret = str_undo_c(&s, '6'); {
        test(ret == -1);
        test(str_len(s) == 5);
        test(str_cap(s) == 18);
        test(strcmp(s, "12345") == 0);
    }

    ret = str_printf(&s, "%d", 2211); {
        test(ret == 0);
        test(str_len(s) == 9);
        test(str_cap(s) == 18);
        test(strcmp(s, "123452211") == 0);
    }

    FILE *f = fopen("test/files/single_line", "r");

    ret = str_getline(&s, f, 30); {
        test(ret == 0);
        test(str_len(s) == 20);
        test(str_cap(s) == 63);
        test(strcmp(s, "123452211single line") == 0);
    }

    ret = str_getline(&s, f, 30); {
        test(ret == -1);
        test(str_len(s) == 20);
        test(str_cap(s) == 63);
        test(strcmp(s, "123452211single line") == 0);
    }

    fclose(f);

    char *test_str = 0;
    ret = str_push_s(&s, test_str); {
        test(ret == -EINVAL);
        test(str_len(s) == 20);
        test(str_cap(s) == 63);
        test(strcmp(s, "123452211single line") == 0);
    }

    str_free(&s); {
        test(!s);
    }

    return test_finish();
}

