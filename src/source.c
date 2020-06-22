#define _POSIX_C_SOURCE 200809L
#include <limits.h> /* PATH_MAX */
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

static size_t file_write(void *ptr, size_t size, size_t nmemb, void *stream) {
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}

static void download(package *pkg, char *url) {
    CURL *curl = curl_easy_init();
    char *name = basename(url);
    FILE *file;

    file = fopenat(pkg->src_dir, name, "wb");

    if (!file) {
        die("[%s] Failed to open %s", pkg->name, name);
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, file_write);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

    if (curl_easy_perform(curl) != 0) {
        remove(name);
        die("[%s] Failed to download source %s", pkg->name, url);
    }

    fclose(file);
    curl_easy_cleanup(curl);
}

static void source_resolve(package *pkg, char *src, char *dest) {
    char *file = basename(src);
    int err = 0;

    if (strstr(src, "://")) {
        if (exists_at(pkg->src_dir, file, 0) == 0) {
            msg("[%s] Found cached source %s", pkg->name, src);

        } else {
            msg("[%s] Downloading %s", pkg->name, src);
            download(pkg, src);
        }

        err = snprintf(dest, PATH_MAX, "%s/%s", pkg->src_dir, file);

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
            err = snprintf(dest, PATH_MAX, "%s/%s", pkg->path, src);
        }
    }

    if (err < 1) {
        die("[%s] Source '%s' does not exist", pkg->name, file);
    }

    if (err >= PATH_MAX) {
        die("[%s] Source path exceeds PATH_MAX", pkg->name);
    }
}

void pkg_source(package *pkg) {
    char *line = 0;
    char *tok;
    FILE *file;
    int i = 0;
    size_t err;

    file = fopenat(pkg->path, "sources", "r");

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

        tok = strtok(line, " 	\r\n");

        if (!tok) {
            die("[%s] Invalid sources file", pkg->name);
        }

        pkg->src[i] = xmalloc(PATH_MAX);
        pkg->des[i] = xmalloc(PATH_MAX);

        source_resolve(pkg, tok, pkg->src[i]);

        tok = strtok(NULL, " 	\r\n");
        tok = tok ? tok : "";

        if (tok[0] == '/') {
            die("[%s] Destination must not be absolute", pkg->name);
        }

        err = strlcpy(pkg->des[i], tok, PATH_MAX);

        if (err >= PATH_MAX) {
            die("strlcpy failed");
        }

        i++;
    }

    free(line);
    fclose(file);
}
