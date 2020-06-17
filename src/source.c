#define _POSIX_C_SOURCE 200809L
#include <libgen.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <curl/curl.h>

#include "source.h"
#include "log.h"
#include "pkg.h"

static size_t file_write(void *ptr, size_t size, size_t nmemb, void *stream) {
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}

void pkg_sources(package *pkg) {
    char *repo = pkg->path[0]; 
    FILE *file;
    char *p_src = 0;
    char *toke;
    char *base;
    char *dest;
    char *src;
    char buf[LINE_MAX];
    char cwd[PATH_MAX + 1];
    char *pwd;
    int len = 0;

    pkg->src_len = 0;
    chdir(repo);
    file = fopen("sources", "r");

    if (chdir(SRC_DIR) != 0) {
        log_error("Sources directory not accessible");
    }

    if (!file) {
        log_error("Sources file invalid");
    }

    // Guess at the length of resulting items based on non-
    // blank lines in file.
    while (fgets(buf, sizeof buf, file) != NULL) {
        if (buf[0] != '#' && buf[0] != '\n') {
            len++;
        }
    }
    rewind(file);

    pkg->source.src  = (char **) malloc(sizeof(char *) * len + 1);
    pkg->source.dest = (char **) malloc(sizeof(char *) * len + 1);

    if (!pkg->source.src || !pkg->source.dest) {
        log_error("Failed to allocate memory");
    }

    while (fgets(buf, sizeof buf, file) != NULL) {
        if (buf[0] == '#' || buf[0] == '\n') {
            continue;
        }

        toke = strtok_r(buf,  " 	\n", &p_src);

        if (!toke) {
            log_error("Sources file invalid");
        }

        src  = strdup(toke);
        base = basename(src);
        toke = strtok_r(NULL, " 	\n", &p_src);
        dest = toke ? strdup(toke) : "";

        pkg->source.dest[pkg->src_len] = malloc(strlen(dest) + 1);
        strcpy(pkg->source.dest[pkg->src_len], dest);

        mkdir(pkg->name, 0777);

        if (chdir(pkg->name) != 0) {
            log_error("Sources directory not accessible");
        }

        if (access(base, F_OK) != -1) {
            log_info("Found cached source %s", base);

        } else if (strncmp(src, "https://", 8) == 0 ||
                   strncmp(src, "http://",  7) == 0) {
            log_info("Downloading %s", src);
            source_download(src);

        } else if (strncmp(src, "git+", 4) == 0) {
            log_warn("Skipping git source (not yet supported) %s", src);

        } else if (chdir(repo) == 0 && 
                   chdir(dirname(src)) == 0 && 
                   access(base, F_OK) != -1) {
            log_info("Found  local source %s/%s", base);

        } else {
            log_error("No local file %s", base);
        }

        pwd = getcwd(cwd, sizeof(cwd));
        pkg->source.src[pkg->src_len] = malloc(strlen(pwd) + strlen(base) + 3);
        strcpy(pkg->source.src[pkg->src_len], pwd);
        strcat(pkg->source.src[pkg->src_len], "/");
        strcat(pkg->source.src[pkg->src_len], base);

        ++pkg->src_len;
        chdir(SRC_DIR);
    }

   fclose(file);
   pkg->source.src[pkg->src_len]  = 0;
   pkg->source.dest[pkg->src_len] = 0;
}

void source_download(char *url) {
    CURL *curl = curl_easy_init();
    char *name = basename(url);
    FILE *file = fopen(name, "wb");

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, file_write);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

    if (curl_easy_perform(curl) != 0) {
        remove(name);
        log_error("Failed to download source %s", url);
    }

    fclose(file);
    curl_easy_cleanup(curl);
}
