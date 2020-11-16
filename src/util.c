#include <unistd.h>
#include <stdint.h>
#include <sys/wait.h>

#include "error.h"
#include "util.h"

char *human_readable(uint64_t n, char out[6]) {
    out[5] = '\0';

    if (n <= 0x400) {
        out[4] = 'B';

    } else if (n <= 0x100000) {
        out[4] = 'K';
        n >>= 10;

    // XX.XM for < 100MB~
    } else if (n < 100000000) {
        out[4] = 'M';
        out[3] = '0' + ((n /= 0x19000) % 10);
        out[2] = '.';
        goto fake;

    } else if (n <= 0x40000000) {
        out[4] = 'M';
        n >>= 20;

    } else if (n <= 0x10000000000) {
        out[4] = 'G';
        n >>= 30;

    } else if (n <= 0x4000000000000) {
        out[4] = 'T';
        n >>= 40;

    } else {
        out[4] = 'P';
        n >>= 50;
    }

    out[3] = '0' + (n % 10);
    out[2] = '0' + ((n /= 10) % 10);
fake:
    out[1] = '0' + ((n /= 10) % 10);
    out[0] = '0' + ((n / 10) % 10);

    return out;
}

int run_cmd(char *const argv[]) {
    pid_t pid = fork();

    if (pid == -1) {
        err_no("failed to fork");
        return -1;

    } else if (pid == 0) {
        execvp(argv[0], argv);

    } else {
        int status = 0;

        waitpid(pid, &status, 0);

        if (WEXITSTATUS(status)) {
            err_no("command exited %d", status);
            return -1;
        }
    }

    return 0;
}

