#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curl/curl.h>

#include "source.h"

static size_t file_write(void *ptr, size_t size, size_t nmemb, void *stream) {
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}

int source_download(char *url) {
    CURL *curl = curl_easy_init();
    FILE *file;
    char *name = basename(url);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, file_write);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

    file = fopen(name, "wb");

    if (file) {
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

        if (curl_easy_perform(curl) != 0) {
            fclose(file);

            printf("error: File failed to download\n");

            if (remove(name) != 0)   
                printf("error: Couldn't remove file\n");

            exit(1);
        }

        fclose(file);
    }

    curl_easy_cleanup(curl);

    return NULL;
}
