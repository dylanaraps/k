#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void sig_add(void (*f)(int)) {
    struct sigaction sa = {
        .sa_handler = f,
    };

    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGINT);

    sigaction(SIGINT,  &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);

    /* atexit(f); */
}
