#include <errno.h>
#include <string.h>

#include "buf.h"
#include "test.h"

int main(int argc, char *argv[]) {
    (void) argc;
    (void) argv;

    int ret = test_begin(__FILE__);

    buf *s = buf_alloc(0, 0); {
        test(s);
        test(buf_len(s) == 0);
        test(buf_cap(s) == 0);
    }

    ret = buf_push_s(&s, "hello, world"); {
        test(ret == 0);
        test(buf_len(s) == 12);
        test(buf_cap(s) == 18);
        test(strcmp(s, "hello, world") == 0);
    }

    buf_set_len(s, 0); {
        test(buf_len(s) == 0);
        test(buf_cap(s) == 18);
        test(s[0] == 0);
        test(strcmp(s, "") == 0);
    }

    ret = buf_push_l(&s, "123456", 6); {
        test(ret == 0);
        test(buf_len(s) == 6);
        test(buf_cap(s) == 18);
        test(strcmp(s, "123456") == 0);
    }

    ret = buf_push_c(&s, '!'); {
        test(ret == 0);
        test(buf_len(s) == 7);
        test(buf_cap(s) == 18);
        test(strcmp(s, "123456!") == 0);
    }

    buf_rstrip(&s, '!'); {
        test(buf_len(s) == 6);
        test(buf_cap(s) == 18);
        test(strcmp(s, "123456") == 0);
    }

    ret = buf_undo_c(&s, '6'); {
        test(ret == 0);
        test(buf_len(s) == 5);
        test(buf_cap(s) == 18);
        test(strcmp(s, "12345") == 0);
    }

    ret = buf_undo_c(&s, '6'); {
        test(ret == -1);
        test(buf_len(s) == 5);
        test(buf_cap(s) == 18);
        test(strcmp(s, "12345") == 0);
    }

    ret = buf_printf(&s, "%d", 2211); {
        test(ret == 0);
        test(buf_len(s) == 9);
        test(buf_cap(s) == 18);
        test(strcmp(s, "123452211") == 0);
    }

    FILE *f = fopen("test/files/single_line", "r");

    ret = buf_getline(&s, f, 30); {
        test(ret == 0);
        test(buf_len(s) == 20);
        test(buf_cap(s) == 63);
        test(strcmp(s, "123452211single line") == 0);
    }

    ret = buf_getline(&s, f, 30); {
        test(ret == -1);
        test(buf_len(s) == 20);
        test(buf_cap(s) == 63);
        test(strcmp(s, "123452211single line") == 0);
    }

    fclose(f);

    char *test_buf = 0;
    ret = buf_push_s(&s, test_buf); {
        test(ret == -EINVAL);
        test(buf_len(s) == 20);
        test(buf_cap(s) == 63);
        test(strcmp(s, "123452211single line") == 0);
    }

    buf_free(&s);

    return test_finish();
}

