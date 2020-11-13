#include <inttypes.h>
#include <stdint.h>

#include "util.h"

char *human_readable(uint64_t n, char out[6]) {
#define KB 1024LL
#define MB (1024LL * KB)
#define GB (1024LL * MB)
#define TB (1024LL * GB)
#define PB (1024LL * TB)
    out[5] = 0;

    // XXXXB
    if (n <= KB) {
        out[4] = 'B';
        out[3] = '0' + (n % 10);
        out[2] = '0' + ((n /= 10) % 10);

    // 0XXXK
    } else if (n <= MB) {
        out[4] = 'K';
        out[3] = '0' + ((n /= KB) % 10);
        out[2] = '0' + ((n /= 10) % 10);

    // XX.XM
    } else if (n < MB * 100) {
        out[4] = 'M';
        out[3] = '0' + ((n /= (KB * 100)) % 10);
        out[2] = '.';

    // XXXXM
    } else if (n <= GB) {
        out[4] = 'M';
        out[3] = '0' + ((n /= MB) % 10);
        out[2] = '0' + ((n /= 10) % 10);

    // XXXXG
    } else if (n <= TB) {
        out[4] = 'G';
        out[3] = '0' + ((n /= GB) % 10);
        out[2] = '0' + ((n /= 10) % 10);

    // XXXXP
    } else if (n <= PB) {
        out[4] = 'T';
        out[3] = '0' + ((n /= TB) % 10);
        out[2] = '0' + ((n /= 10) % 10);

    } else {
        out[4] = 'P';
        out[3] = '0' + ((n /= PB) % 10);
        out[2] = '0' + ((n /= 10) % 10);
    }

    out[1] = '0' + ((n /= 10) % 10);
    out[0] = '0' + ((n / 10) % 10);

    return out;
}

