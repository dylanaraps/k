#define _POSIX_C_SOURCE 200809L
#include <limits.h> /* PATH_MAX, LINE_MAX */
#include <unistd.h> /* chdir */
#include <string.h> /* strcspn */
#include <libgen.h> /* basename */

#include <curl/curl.h>

#include "log.h"
#include "util.h"
#include "strl.h"
#include "pkg.h"
#include "cache.h"
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
void source_init(package *pkg) {
    char *tmp;

    if (chdir(CAC_DIR) != 0) {
        die("Cache directory is not accessible");
    }
    
    if (chdir("../sources") != 0) {
        die("[%s] Source directory is not accessible", pkg->name); 
    }

    mkdir_p(pkg->name);

    if (chdir(pkg->name) != 0) {
        die("[%s] Source directory is not accessible", pkg->name); 
    }

    tmp = getcwd(pkg->src_dir, PATH_MAX);

    if (!tmp) {
        die("[%s] Failed to init source directory", pkg->name);
    }
}

static void source_resolve(package *pkg, char *src, char *dest) {
    char *file = basename(src);
    int err = 0;

    if (chdir(pkg->path[0]) != 0) {
        die("[%s] Repository directory is not accessible", pkg->name); 
    }

    if (access(src, F_OK) != -1) {
        err = snprintf(dest, PATH_MAX, "%s/%s", pkg->path[0], file);     
    }

    if (chdir(pkg->src_dir) != 0) {
        die("[%s] Source directory is not accessible", pkg->name); 
    }

    if (access(file, F_OK) != -1) {
        err = snprintf(dest, PATH_MAX, "%s/%s", pkg->src_dir, file);     
    }

    if (err < 1) {
        die("[%s] Source '%s' does not exist", pkg->name, file);
    }

    if (err > PATH_MAX) {
        die("[%s] Source path exceeds PATH_MAX", pkg->name);
    }
}

void pkg_source(package *pkg) {
    char line[LINE_MAX];
    char *tok;
    FILE *file;
    int i = 0;
    int len;
    int err;

    source_init(pkg);

    if (chdir(pkg->path[0]) != 0) {
        die("[%s] Repository is not accessible (%s)", pkg->name, pkg->path[0]);
    }

    file = fopen("sources", "r");

    if (!file) {
        die("[%s] Failed to open sources file", pkg->name);
    }

    pkg->src_l = cntlines(file);

    /* empty sources file */
    if (pkg->src_l == 0) {
        goto end;
    }

    pkg->src = xmalloc((pkg->src_l + 1) * sizeof(char *));
    pkg->des = xmalloc((pkg->src_l + 1) * sizeof(char *));

    while (fgets(line, LINE_MAX, file) && i < pkg->src_l) { 
        if (line[0] == '#' || line[0] == '\n') {
            continue;
        }

        tok = strtok(line, " 	\r\n");

        if (!tok) {
            die("[%s] Invalid sources file", pkg->name);
        }

        len = strlen(tok) + 1;
        pkg->src[i] = xmalloc(len);
        err = snprintf(pkg->src[i], len, "%s", basename(tok));

        if (err == -1) {
            die("Failed to construct cache directory");
        }

        source_resolve(pkg, tok, pkg->src[i]);
        /* printf("%s\n", pkg->src[i]); */

        /* tok = strtok(NULL, " 	\r\n"); */

        /* /1* ensure optional field is not null *1/ */
        /* tok = tok ? tok : ""; */

        /* len = strlen(tok) + 1; */
        /* pkg->des[i] = xmalloc(len); */
        /* err = strlcpy(pkg->des[i], tok, len); */

        /* if (err > len) { */
        /*     die("strlcpy was truncated"); */
        /* } */

        i++;
    }

end:
    fclose(file);
}
