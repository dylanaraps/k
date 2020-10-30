#include <stdio.h>

#include <curl/curl.h>

#include "util.h"
#include "download.h"

int source_download(const char *url, const char *dest) {
    FILE *file = fopen(dest, "w");

    if (!file) {
        err_no("failed to open file '%s'", dest);
        return -1;
    }

    CURLcode ret = curl_global_init(CURL_GLOBAL_ALL);

    if (ret != 0) {
        err("failed to initialize curl: %s", curl_easy_strerror(ret));
        goto file_err;
    }

    CURL *curl = curl_easy_init();

    if (!curl) {
        err("failed to initialize curl");
        goto file_err;
    }

    ret = curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

    if (ret != 0) {
        err("failed to set CURLOPT_WRITEDATA: %s", 
            curl_easy_strerror(ret));
        goto curl_err;
    }

    ret = curl_easy_setopt(curl, CURLOPT_URL, url);

    if (ret != 0) {
        err("failed to set CURLOPT_URL to '%s': %s", url,
            curl_easy_strerror(ret));
        goto curl_err;
    }

    ret = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

    if (ret != 0) {
        err("failed to set CURLOPT_NOPROGRESS: %s", 
            curl_easy_strerror(ret));
        goto curl_err;
    }

    ret = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    if (ret != 0) {
        err("failed to set CURLOPT_FOLLOWLOCATION: %s", 
            curl_easy_strerror(ret));
        goto curl_err;
    }

    ret = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);

    if (ret != 0) {
        err("failed to set CURLOPT_WRITEFUNCTION: %s", 
            curl_easy_strerror(ret));
        goto curl_err;
    }

    ret = curl_easy_perform(curl);

    if (ret != 0) {
        err("failed to download file '%s': %s", url,
            curl_easy_strerror(ret));
        goto curl_err;
    }

    curl_easy_cleanup(curl);
    fclose(file);

    return 0;     

curl_err:
    curl_easy_cleanup(curl);
file_err:
    fclose(file);

    return -1;
}

