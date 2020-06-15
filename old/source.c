#define _POSIX_C_SOURCE 200809L
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <curl/curl.h>

#include "path.h"
#include "util.h"
#include "mkdir.h"
#include "kiss.h"
#include "source.h"

static size_t file_write(void *ptr, size_t size, size_t nmemb, void *stream) {
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}

int source_download(char *url) {
    CURL *curl = curl_easy_init();
    char *name = basename(url);
    FILE *file = fopen(name, "wb");

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, file_write);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

    if (!file) {
        goto done;
    }

    if (curl_easy_setopt(curl, CURLOPT_WRITEDATA, file)) {
        remove(name);
    }

done:
    fclose(file);
    curl_easy_cleanup(curl);
    return 0;
}

int parse_sources(char *pkg) {
   char *repo_dir = path_find(pkg, 0); 
   char *sources  = strjoin(repo_dir, "sources", "/");
   char *src_dir  = strjoin(SRC_DIR, pkg, "/");
   char *dest, *source, *local;
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

   while ((llen = getline(&lbuf, &lsiz, file)) > 0) {
       // Drop newlines.
       if ((lbuf)[llen - 1] == '\n') {
           (lbuf)[llen - 1] = '\0';
       }

       // Skip comments and blank lines.
       if ((lbuf)[0] == '#' || (lbuf)[0] == '\n') {
           continue;
       }

       if (mkpath(src_dir) != 0) {
           printf("%s (%s)\n", pkg, "Couldn't create source directory");
           exit(1);
       }

       source = strtok(lbuf, " 	"); 
       dest   = strjoin(src_dir, basename(source), "/");
       local  = strjoin(repo_dir, source, "/");

       if (access(dest, F_OK) != -1) {
           printf("%s (Found cached source %s)\n", pkg, dest);
        
       } else if (strncmp(source, "https://", 8) == 0 ||
           strncmp(source, "http://",  7) == 0) {
           printf("%s (Downloading %s)\n", pkg, source);
           source_download(source);

       } else if (access(local, F_OK) != -1) {
           printf("%s (Found local source %s)\n", pkg, local);

       } else {
           printf("%s (No local file %s)\n", pkg, local);
           exit(1);
       }

       free(dest);
       lbuf=NULL;
   }

   fclose(file);
   return 0; 
}
