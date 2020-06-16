#define _POSIX_C_SOURCE 200809L
#include <libgen.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <curl/curl.h>

#include "source.h"
#include "pkg.h"

static size_t file_write(void *ptr, size_t size, size_t nmemb, void *stream) {
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}

void pkg_sources(package *pkg) {
   char **repos = pkg->path; 
   FILE *file;
   char *lbuf = 0;
   char *source;
   char *name;
   char *dest;
   char cwd[PATH_MAX];
   char *pwd;

   pkg->src_len = 0;
   chdir(repos[0]);
   file = fopen("sources", "r");
   
   if (chdir(SRC_DIR) != 0) {
       printf("error: Sources directory not accessible\n"); 
       exit(1);
   }

   if (!file) {
       printf("error: Sources file invalid\n");
       exit(1);
   }

   pkg->source.src  = (char **) malloc(sizeof(char*));
   pkg->source.dest = (char **) malloc(sizeof(char*));

   while ((getline(&lbuf, &(size_t){0}, file)) > 0) {
       // Skip comments and blank lines.
       if ((lbuf)[0] == '#' || (lbuf)[0] == '\n') {
           continue;
       }

       source = strtok(lbuf, " 	\n");
       dest   = strtok(NULL, " 	\n");
       name   = basename(source);

       if (dest == NULL) {
           dest = "";
       }

       mkdir(pkg->name, 0777);

       if (chdir(pkg->name) != 0) {
           printf("error: Sources directory not accessible\n");
           exit(1);
       }

       if (access(name, F_OK) != -1) {
           printf("%s (Found cached source %s)\n", pkg->name, name);
        
       } else if (strncmp(source, "https://", 8) == 0 ||
                  strncmp(source, "http://",  7) == 0) {
           printf("%s (Downloading %s)\n", pkg->name, source);
           source_download(source);

       } else if (strncmp(source, "git+", 4) == 0) {
           printf("%s (Skipping git source.. %s)\n", pkg->name, source);

       } else if (chdir(*repos) == 0 && 
                  chdir(dirname(source)) == 0 && 
                  access(name, F_OK) != -1) {
           printf("%s (Found local source %s)\n", pkg->name, name);

       } else {
           printf("error: No local file %s\n", source);
           exit(1);
       }

       pwd = getcwd(cwd, sizeof(cwd));
       pkg->source.src[pkg->src_len]  = malloc(strlen(pwd) + strlen(name) + 3);
       pkg->source.dest[pkg->src_len] = malloc(strlen(dest) + 1);

       if (pkg->source.src[pkg->src_len] == NULL ||
           pkg->source.dest[pkg->src_len] == NULL) {
           printf("Failed to allocate memory\n");
           exit(1);
       }

       strcpy(pkg->source.src[pkg->src_len], pwd);
       strcat(pkg->source.src[pkg->src_len], "/");
       strcat(pkg->source.src[pkg->src_len], name);
       strcpy(pkg->source.dest[pkg->src_len], dest);
       ++pkg->src_len;

       chdir(SRC_DIR);
   }

   pkg->source.src[pkg->src_len + 1] = 0;
   pkg->source.dest[pkg->src_len + 1] = 0;
   free(lbuf);
   fclose(file);
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
