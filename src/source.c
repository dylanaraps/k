#define _POSIX_C_SOURCE 200809L
#include <limits.h> /* PATH_MAX, LINE_MAX */
#include <unistd.h> /* chdir */
#include <string.h> /* strcspn */

#include <curl/curl.h>

#include "log.h"
#include "util.h"
#include "strl.h"
#include "pkg.h"
#include "source.h"

/* static size_t file_write(void *ptr, size_t size, size_t nmemb, void *stream) { */
/*     size_t written = fwrite(ptr, size, nmemb, (FILE *)stream); */
/*     return written; */
/* } */

/* static void download(char *url) { */
    /* CURL *curl = curl_easy_init(); */
    /* char *name = basename(url); */
    /* FILE *file = xfopen(name, "wb"); */

    /* curl_easy_setopt(curl, CURLOPT_URL, url); */
    /* curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, file_write); */
    /* curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L); */
    /* curl_easy_setopt(curl, CURLOPT_WRITEDATA, file); */

    /* if (curl_easy_perform(curl) != 0) { */
    /*     remove(name); */
    /*     die("Failed to download source %s", url); */
    /* } */

    /* fclose(file); */
    /* curl_easy_cleanup(curl); */
/* } */

void pkg_source(package *pkg) {
    char line[LINE_MAX];
    char *tok;
    FILE *file;
    int i = 0;
    int len;
    int err;

    if (chdir(pkg->path[0]) != 0) {
        die("[%s] Repository is not accessible", pkg->name);
    }

    file = fopen("sources", "r");

    if (!file) {
        die("[%s] Failed to open sources file", pkg->name);
    }

    /* guess at the file length */
    while (fgets(line, LINE_MAX, file)) {
        if (line[0] != '#' && line[0] != '\n') {
            pkg->src_l++;
        }
    }
    rewind(file);

    pkg->src = xmalloc((pkg->src_l + 1) * sizeof(char *));
    pkg->des = xmalloc((pkg->src_l + 1) * sizeof(char *));

    while (fgets(line, LINE_MAX, file)) { 
        if (line[0] == '#' || line[0] == '\n') {
            continue;
        }

        tok = strtok(line, " 	\r\n");

        if (!tok) {
            die("[%s] Invalid sources file", pkg->name);
        }

        len = strlen(tok) + 1;
        pkg->src[i] = xmalloc(len);
        err = strlcpy(pkg->src[i], tok, len);

        if (err > len) {
            die("strlcpy was truncated");
        }

        tok = strtok(NULL, " 	\r\n");

        /* ensure optional field is not null */
        tok = tok ? tok : "";

        len = strlen(tok) + 1;
        pkg->des[i] = xmalloc(len);
        err = strlcpy(pkg->des[i], tok, len);

        if (err > len) {
            die("strlcpy was truncated");
        }

        i++;
    }

    fclose(file);
}
