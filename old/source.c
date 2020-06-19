#define _POSIX_C_SOURCE 200809L
#include <libgen.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <curl/curl.h>

#include "source.h"
#include "log.h"
#include "util.h"
#include "pkg.h"

static size_t file_write(void *ptr, size_t size, size_t nmemb, void *stream) {
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}

static void download(char *url) {
    CURL *curl = curl_easy_init();
    char *name = basename(url);
    FILE *file = xfopen(name, "wb");

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, file_write);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

    if (curl_easy_perform(curl) != 0) {
        remove(name);
        die("Failed to download source %s", url);
    }

    fclose(file);
    curl_easy_cleanup(curl);
}

void pkg_sources(package *pkg) {
    char *repo = pkg->path[0]; 
    FILE *file;
    char *toke;
    char *base;
    char *dest;
    char *src;
    char *buf = 0;
    char cwd[PATH_MAX];
    char *pwd;
    int len = 0;

    pkg->src_len = 0;
    xchdir(repo);
    file = xfopen("sources", "r");

    xchdir(SRC_DIR);

    // Guess at the length of resulting items based on non-
    // blank lines in file.
    while (getline(&buf, &(size_t){0}, file) != -1)
        if (buf[0] != '#' && buf[0] != '\n')
            len++;
    rewind(file);

    pkg->src  = xmalloc(sizeof(char *) * (len + 1));
    pkg->dest = xmalloc(sizeof(char *) * (len + 1));

    while ((getline(&buf, &(size_t){0}, file) != -1)) {
        if (buf[0] == '#' || buf[0] == '\n')
            continue;

        toke = strtok(buf,  " 	\n");

        if (!toke)
            die("Sources file invalid");

        src  = strdup(toke);
        base = basename(src);
        toke = strtok(NULL, " 	\n");
        dest = toke ? strdup(toke) : "";

        pkg->dest[pkg->src_len] = xmalloc(strlen(dest) + 1);
        strcpy(pkg->dest[pkg->src_len], dest);

        mkdir(pkg->name, 0777);
        xchdir(pkg->name);

        if (access(base, F_OK) != -1) {
            msg("[%s] Found cached source %s", pkg->name, base);

        } else if (strncmp(src, "https://", 8) == 0 ||
                   strncmp(src, "http://",  7) == 0) {
            msg("[%s] Downloading %s", pkg->name, src);
            download(src);

        } else if (strncmp(src, "git+", 4) == 0) {
            die("Skipping git source (not yet supported) %s", src);

        } else if (chdir(repo) == 0 && 
                   chdir(dirname(src)) == 0 && 
                   access(base, F_OK) != -1) {
            msg("[%s] Found local source %s", pkg->name, base);

        } else {
            die("[%s] No local file %s", pkg->name, base);
        }

        pwd = getcwd(cwd, sizeof(cwd));
        pkg->src[pkg->src_len] = xmalloc(strlen(pwd) + strlen(base) + 5);
        sprintf(pkg->src[pkg->src_len], "%s/%s", pwd, base);

        pkg->src_len++;
        xchdir(SRC_DIR);
    }

    free(buf);
    fclose(file);
    pkg->src[pkg->src_len]  = 0;
    pkg->dest[pkg->src_len] = 0;
}
