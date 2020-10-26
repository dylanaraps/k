#include <errno.h>
#include <string.h>

#include <curl/curl.h>

#include "util.h"
#include "download.h"

int source_download(const char *url, const char *dest) {
    if (!url || !url[0] || !dest || !dest[0]) {
        return -1; 
    }

    FILE *file = fopen(dest, "w");

    if (!file) {
        die("failed to open file %s: %s", dest, strerror(errno));
    }

    CURL *curl = curl_easy_init();

    if (!curl) {
        die("failed to initialize curl"); 
    }

    if (curl_easy_setopt(curl, CURLOPT_WRITEDATA, file) != 0 ||
        curl_easy_setopt(curl, CURLOPT_URL, url) != 0 ||
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L) != 0 ||
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L) != 0 ||
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite) != 0) {
        die("failed to configure curl");
    }

    CURLcode err = curl_easy_perform(curl);

    fclose(file);
    curl_easy_cleanup(curl);

    if (err != 0) {
        remove(dest);
        return -1;
    }

    return 0;     
}

