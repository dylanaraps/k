#define _POSIX_C_SOURCE 200809L
#include <limits.h>
#include <string.h>
#include <libgen.h>
#include <fcntl.h>

#include <curl/curl.h>

#include "log.h"
#include "util.h"
#include "pkg.h"
#include "cache.h"
#include "source.h"

static size_t file_write(void *ptr, size_t size, size_t nmemb, void *stream) {
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}

static void download(package *pkg, char *url) {
    CURL *curl;
    char tmp[PATH_MAX];
    char *name;
    FILE *file;

    if (!url) {
        die("null url");
    }

    curl = curl_easy_init();

    if (!curl) {
        die("Failed to initialize curl");
    }

    xstrlcpy(tmp, url, PATH_MAX);
    name = basename(tmp);

    if (!name) {
        die("Failed to construct basename");
    }

    file = fopenat(pkg->src_dir, name, O_RDWR | O_CREAT, "wb");

    if (!file) {
        die("[%s] Failed to open %s/%s", pkg->name, pkg->src_dir, name);
    }

    if (curl_easy_setopt(curl, CURLOPT_URL, url) != 0) {
        die("Failed to set CURLOPT_URL");
    }

    if (curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, file_write) != 0) {
        die("Failed to set CURLOPT_WRITEFUNCTION");
    }

    if (curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L) != 0) {
        die("Failed to set CURLOPT_NOPROGRESS");
    }

    if (curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L) != 0) {
        die("Failed to set CURLOPT_FOLLOWLOCATION");
    }

    if (curl_easy_setopt(curl, CURLOPT_WRITEDATA, file) != 0) {
        die("Failed to set CURLOPT_WRITEDATA");
    }

    if (curl_easy_perform(curl) != 0) {
        remove(name);
        die("[%s] Failed to download source %s", pkg->name, url);
    }

    fclose(file);
    curl_easy_cleanup(curl);
}

static void source_resolve(package *pkg, char *src, char **dest) {
    char *file = basename(src);

    if (strstr(src, "://")) {
        if (exists_at(pkg->src_dir, file, 0) == 0) {
            msg("[%s] Found cached source %s", pkg->name, src);

        } else {
            msg("[%s] Downloading %s", pkg->name, src);
            download(pkg, src);
        }

        *dest = xmalloc(PATH_MAX);
        xsnprintf(*dest, PATH_MAX, "%s/%s", pkg->src_dir, file);

    } else if (strncmp(src, "git+", 4) == 0) {
        die("[%s] Found git source (not yet supported) %s", pkg->name, src);

        /* libgit2? or... git commands directly. */
        /* libgit2 requires cmake which is too much for core. */
        /* git commands lose the benefit of static compilation... */
        /* welp. no shallow clones in libgit2. */
        /* https://github.com/libgit2/libgit2/issues/3058 */

    } else {
        if (exists_at(pkg->path, src, 0) == 0) {
            msg("[%s] Found  local source %s", pkg->name, src);
            *dest = xmalloc(PATH_MAX);
            xsnprintf(*dest, PATH_MAX, "%s/%s", pkg->path, src);

        } else {
            die("[%s] Source '%s' does not exist", pkg->name, file);
        }
    }
}

void pkg_source(package *pkg) {
    char *line = 0;
    char *tmp;
    FILE *file;
    int i = 0;

    file = fopenat(pkg->path, "sources", O_RDONLY, "r");

    if (!file) {
        die("[%s] Failed to open sources file", pkg->name);
    }

    pkg->src_l = cntlines(file);

    if (pkg->src_l == 0) {
        die("[%s] Empty sources file", pkg->name);
    }

    pkg->src = xmalloc((pkg->src_l + 1) * sizeof(char *));
    pkg->des = xmalloc((pkg->src_l + 1) * sizeof(char *));

    while (getline(&line, &(size_t){0}, file) != -1) {
        if (line[0] == '#' || line[0] == '\n') {
            continue;
        }

        if (i > pkg->src_l) {
            die("[%s] Mismatch in source parser", pkg->name);
        }

        split_in_two(line, " 	\r\n", &tmp, &pkg->des[i]);

        if (!tmp) {
            die("[%s] Invalid sources file", pkg->name);
        }

        if (pkg->des[i][0] == '/') {
            die("[%s] Destination must not be absolute", pkg->name);
        }

        source_resolve(pkg, tmp, &pkg->src[i]);
        free(tmp);

        i++;
    }

    free(line);
    fclose(file);
}
