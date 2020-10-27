#include <stdio.h>

#include <curl/curl.h>

#include "download.h"

int source_download(const char *url, const char *dest) {
    FILE *file = fopen(dest, "w");

    if (!file) {
        return -1;
    }

    CURL *curl = curl_easy_init();

    if (!curl) {
        return -1;
    }

    if (curl_easy_setopt(curl, CURLOPT_WRITEDATA, file) != 0 ||
        curl_easy_setopt(curl, CURLOPT_URL, url) != 0 ||
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L) != 0 ||
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L) != 0 ||
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite) != 0) {
        return -1;
    }

    CURLcode err = curl_easy_perform(curl);

    fclose(file);
    curl_easy_cleanup(curl);

    return err == 0 ? 0 : -1;     
}

