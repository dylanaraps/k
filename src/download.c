/**
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
 */
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#ifdef USE_CURL
#include <curl/curl.h>
#endif

#include "error.h"
#include "util.h"
#include "download.h"

/**
 * CURL is used to download remote sources. This dependency can be disabled
 * by setting the environment variable CURL to 0 prior to the build. Disabling
 * this dependency will also disable the download feature.
 */
#ifdef USE_CURL

// same handle is used for all requests
static CURL *curl;

// catch Ctrl+C
static volatile sig_atomic_t sigint;

static void handle_sigint(int sig) {
    (void) sig;
    sigint = 1;
}

static int status(void *p, curl_off_t tot, curl_off_t cur,
                           curl_off_t a, curl_off_t b) {
    (void) a;
    (void) b;

    char c[6];
    char t[6];

#define BAR_LEN 25
    int elapsed = (cur * BAR_LEN) / (tot ? tot : 1);

    fprintf(stdout, "%-40s %s / %s [%.*s%*s] %3d%%\r",
        (char *) p,
        human_readable(cur, c),
        human_readable(tot, t),
        elapsed,
        "================================",
        BAR_LEN - elapsed,
        "",
        (int) ((cur * 100) / (tot ? tot : 1))
    );
    fflush(stdout);

    return sigint ? -1 : 0;
}

static int source_curl_init(void) {
    CURLcode ret = curl_global_init(CURL_GLOBAL_ALL);

    if (ret != 0) {
        err("failed to initialize curl: %s", curl_easy_strerror(ret));
        return -1;
    }

    if (!(curl = curl_easy_init())) {
        err("failed to initialize curl");
        goto error;
    }

    struct sigaction act = { .sa_handler = handle_sigint };

    if (sigaction(SIGINT, &act, NULL) == -1) {
        err_no("sigaction failed");
        goto error;
    }

    return 0;

error:
    curl_global_cleanup();
    return -1;
}

static CURLcode source_curl_setopts(void) {
    CURLcode ret = 0;

    if ((ret = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L)) != 0) {
        err("CURLOPT_NOPROGRESS: %s", curl_easy_strerror(ret));
        return ret;
    }

    if ((ret = curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L)) != 0) {
        err("CURLOPT_TCP_KEEPALIVE: %s", curl_easy_strerror(ret));
        return ret;
    }

    if ((ret = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L)) != 0) {
        err("CURLOPT_FOLLOWLOCATION: %s", curl_easy_strerror(ret));
        return ret;
    }

    if ((ret = curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, status)) != 0) {
        err("CURLOPT_XFERINFOFUNCTION: %s", curl_easy_strerror(ret));
        return ret;
    }

    return ret;
}

static CURLcode source_curl_stage(const char *url, FILE *dest) {
    CURLcode ret = 0;

    if ((ret = curl_easy_setopt(curl, CURLOPT_URL, url)) != 0) {
        err("CURLOPT_URL: %s", curl_easy_strerror(ret));
        return ret;
    }

    if ((ret = curl_easy_setopt(curl, CURLOPT_WRITEDATA, dest)) != 0) {
        err("CURLOPT_WRITEDATA: %s", curl_easy_strerror(ret));
        return ret;
    }

    char *basename = strrchr(url, '/') + 1;

    if ((ret = curl_easy_setopt(curl, CURLOPT_XFERINFODATA, basename)) != 0) {
        err("CURLOPT_XFERINFODATA: %s", curl_easy_strerror(ret));
        return ret;
    }

    return ret;
}

int source_download(const char *url, FILE *dest) {
    if (!curl) {
        if (source_curl_init() != 0) {
            return -1;
        }

        if (source_curl_setopts() != 0) {
            return -1;
        }
    }

    if (source_curl_stage(url, dest) != 0) {
        return -1;
    }

    CURLcode ret = curl_easy_perform(curl);

    if (ret != 0) {
        err("failed to download file '%s': %s", url, curl_easy_strerror(ret));
        return -1;
    }

    fputc('\n', stdout);
    return 0;
}

void source_curl_cleanup(void) {
    if (curl) {
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
}

/**
 * Fallback stubs for public functions when CURL dependency has been disabled.
 * In the future, the build system may simply exclude this file from compilation
 * with ifdefs used around the caller.
 */
#else

int source_download(const char *url, FILE *dest) {
    (void) url; (void) dest;

    err("package manager compiled without download support");
    return -ENOSYS;
}

void source_curl_cleanup(void) {
    //
}

#endif // USE_CURL

