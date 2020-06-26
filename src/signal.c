#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "cache.h"
#include "pkg.h"
#include "repo.h"
#include "signal.h"

void sig_init(void) {
    struct sigaction sa = {
        .sa_handler = sig_hand,
    };

    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGINT);

    sigaction(SIGINT,  &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
}

void sig_hand(int i) {
    (void)(i);

    cache_destroy();
    repo_destroy();
    pkg_destroy_all();
}
