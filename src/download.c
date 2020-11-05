/**
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
 */
#include <curl/curl.h>

#include "error.h"
#include "download.h"

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

