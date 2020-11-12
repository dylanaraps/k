/**
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
 */
#include <stdlib.h>
#include <string.h>

#ifdef USE_CURL
#include <curl/curl.h>
#endif

#include "error.h"
#include "download.h"

/**
 * CURL is used to download remote sources. This dependency can be disabled
 * by setting the environment variable CURL to 0 prior to the build. Disabling
 * this dependency will also disable the download feature.
 */
#ifdef USE_CURL

// same handle is used for all requests
static CURL *curl = 0;

static int source_curl_init(void) {
    CURLcode ret = curl_global_init(CURL_GLOBAL_ALL);

    if (ret != 0) {
        err("failed to initialize curl: %s", curl_easy_strerror(ret));
        return -1;
    }

    curl = curl_easy_init();

    if (!curl) {
        curl_global_cleanup();
        err("failed to initialize curl");
        return -1;
    }

    return 0;
}

static int curl_s(void *p,
    curl_off_t D, curl_off_t d, curl_off_t U, curl_off_t u) {

#define BAR_LEN 30

    // unused
    (void) u;
    (void) U;

    // values could still be 0.
    if (d > 1 && D > 1) {
        curl_off_t perc = d * 100 / D;
        curl_off_t elap = d * BAR_LEN / D;

        printf("%-40s (%" CURL_FORMAT_CURL_OFF_T " / %"
            CURL_FORMAT_CURL_OFF_T ") KiB [", (char *) p, d, D);

        for (curl_off_t i = 0; i < BAR_LEN; i++) {
            putchar(i < elap ? '=' : ' ');
        }

        printf("] %" CURL_FORMAT_CURL_OFF_T "%%\r", perc);
    }

    return 0;
}

static CURLcode source_curl_setopts(void) {
    CURLcode ret = 0;

    if ((ret = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L)) != 0) {
        err("CURLOPT_NOPROGRESS: %s", curl_easy_strerror(ret));
        return ret;
    }

    if ((ret = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L)) != 0) {
        err("CURLOPT_FOLLOWLOCATION: %s", curl_easy_strerror(ret));
        return ret;
    }

    if ((ret = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL)) != 0) {
        err("CURLOPT_WRITEFUNCTION: %s", curl_easy_strerror(ret));
        return ret;
    }

    if ((ret = curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, curl_s)) != 0) {
        err("CURLOPT_PROGRESSFUNCTION: %s", curl_easy_strerror(ret));
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

    char *base = strrchr(url, '/') + 1;

    if ((ret = curl_easy_setopt(curl, CURLOPT_XFERINFODATA, base)) != 0) {
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

    return 0;
}

void source_curl_cleanup(void) {
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

/**
 * Fallback stubs for public functions when CURL dependency has been disabled.
 * In the future, the build system may simply exclude this file from compilation
 * with ifdefs used around the caller.
 */
#else

int source_download(const char *url, FILE *dest) {
    (void) url;
    (void) dest;

    return -ENOSYS;
}

void source_curl_cleanup(void) {
    //
}

#endif // USE_CURL

