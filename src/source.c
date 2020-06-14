#define _POSIX_C_SOURCE 200809L
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <curl/curl.h>

#include "find.h"
#include "util.h"
#include "source.h"

static size_t file_write(void *ptr, size_t size, size_t nmemb, void *stream) {
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}

int source_download(char *url, char *path) {
    CURL *curl = curl_easy_init();
    char *name = basename(url);
    FILE *file = fopen(name, "wb");
    char *dest = strjoin(path, name, "/");
    printf("%s\n", dest);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, file_write);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

    if (file) {
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

        if (curl_easy_perform(curl) != 0) {
            remove(name);
            fclose(file);
            curl_easy_cleanup(curl);
            return 1;
        }
    }

    fclose(file);
    curl_easy_cleanup(curl);
    return 0;
}

int parse_sources(char *pkg) {
   char *sources = find_file(pkg, "sources");
   FILE *file;
   size_t  lsiz=0;
   char*   lbuf=0;
   ssize_t llen=0;

   if (!sources) {
       return 1;
   }

   file = fopen(sources, "r");

   if (!file) {
       fclose(file);
       return 1;
   }

   while ((llen=getline(&lbuf, &lsiz, file)) > 0) {
       char *source = strtok(lbuf, " 	"); 
       char *target = strtok(NULL, " 	"); 
       printf("Downloading %s\n", source);
       source_download(source, target);
       free(lbuf);
       lbuf=NULL;
   }

   fclose(file);
   return 0; 
}
