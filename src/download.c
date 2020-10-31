#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include <curl/curl.h>

#include "cache.h"
#include "util.h"
#include "download.h"

int source_download(const char *url, int dest_fd) {
    char *basename = strrchr(url, '/');

    if (!basename || basename[0] != '/') {
        return -1;
    }

    FILE *file = fopenat(dest_fd, basename + 1, O_RDWR | O_CREAT, "w");

    if (!file) {
        err_no("failed to open file '%s'", basename + 1);
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

int source_type(const char *url) {
    if (url[0] == '/') {
        return SRC_ABS; 

    } else if (strncmp(url, "git+", 4) == 0) {
        return SRC_GIT; 

    } else if (strstr(url, "://")) {
        return SRC_URL;

    } else {
        return SRC_REL;
    }
}

