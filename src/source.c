#define _POSIX_C_SOURCE 200809L
#include <libgen.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <curl/curl.h>

#include "util.h"
#include "source.h"

static size_t file_write(void *ptr, size_t size, size_t nmemb, void *stream) {
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}

void source_download(char *url) {
    CURL *curl = curl_easy_init();
    char *name = basename(url);
    FILE *file = fopen(name, "wb");
    char cwd[PATH_MAX];

    printf("%s\n", strdup(getcwd(cwd, sizeof(cwd))));

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, file_write);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

    if (curl_easy_perform(curl) != 0) {
        remove(name);
        printf("error: failed to download source\n");
        exit(1);
    }

    fclose(file);
    curl_easy_cleanup(curl);
}

