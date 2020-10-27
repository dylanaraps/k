#include <stdio.h>

#include <curl/curl.h>

#include "util.h"
#include "download.h"

int source_download(const char *url, const char *dest) {
    FILE *file = fopen(dest, "w");

    if (!file) {
        err("failed to open file '%s': %s", dest, strerror(errno));
        return -1;
    }

    CURLcode ret = curl_global_init(CURL_GLOBAL_ALL);

    if (ret != 0) {
        err("failed to initialize curl: %s", curl_easy_strerror(ret));
        return -1;
    }

    CURL *curl = curl_easy_init();

    if (!curl) {
        err("failed to initialize curl");
        return -1;
    }

    ret = curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

    if (ret != 0) {
        err("failed to set CURLOPT_WRITEDATA: %s", 
            curl_easy_strerror(ret));
        return -1;
    }

    ret = curl_easy_setopt(curl, CURLOPT_URL, url);

    if (ret != 0) {
        err("failed to set CURLOPT_URL to '%s': %s", url,
            curl_easy_strerror(ret));
        return -1;
    }

    ret = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

    if (ret != 0) {
        err("failed to set CURLOPT_NOPROGRESS: %s", 
            curl_easy_strerror(ret));
        return -1;
    }

    ret = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    if (ret != 0) {
        err("failed to set CURLOPT_FOLLOWLOCATION: %s", 
            curl_easy_strerror(ret));
        return -1;
    }

    ret = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);

    if (ret != 0) {
        err("failed to set CURLOPT_WRITEFUNCTION: %s", 
            curl_easy_strerror(ret));
        return -1;
    }

    ret = curl_easy_perform(curl);

    fclose(file);
    curl_easy_cleanup(curl);

    if (ret != 0) {
        err("failed to download file '%s': %s", url,
            curl_easy_strerror(ret));
        return -1;
    }

    return 0;     
}

