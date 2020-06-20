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

static size_t file_write(void *ptr, size_t size, size_t nmemb, void *stream) {
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}

static void download(package *pkg, char *url) {
    CURL *curl = curl_easy_init();
    char *name = basename(url);
    FILE *file;

    if (chdir(pkg->src_dir) != 0) {
        die("[%s] Source cache not accessible");
    }

    file = fopen(name, "wb");

    if (!file) {
        die("Failed to open %s", name);
    }

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

void source_init(package *pkg) {
    char *tmp;

    if (chdir(CAC_DIR) != 0) {
        die("Cache directory is not accessible");
    }

    if (chdir("../sources") != 0) {
        die("Source directory is not accessible");
    }

    mkdir_p(pkg->name);

    if (chdir(pkg->name) != 0) {
        die("Source directory is not accessible");
    }

    tmp = getcwd(pkg->src_dir, PATH_MAX);

    if (!tmp) {
        die("Failed to init source directory");
    }
}

static void source_resolve(package *pkg, char *src, char *dest) {
    char *file = basename(src);
    int err = 0;

    if (strncmp(src, "https://", 8) == 0 ||
        strncmp(src, "http://",  7) == 0) {

        if (chdir(pkg->src_dir) != 0) {
            die("Source directory is not accessible");
        }

        if (access(file, F_OK) != -1) {
            msg("Found cached source %s", src);

        } else {
            msg("Downloading %s", src);
            download(pkg, src);
        }

        err = snprintf(dest, PATH_MAX, "%s/%s", pkg->src_dir, file);
        goto end;

    } else if (strncmp(src, "git+", 4) == 0) {
        die("Found git source (not yet supported) %s", src);

        /* libgit2? or... git commands directly. */
        /* libgit2 requires cmake which is too much for core. */
        /* git commands lose the benefit of static compilation... */
        /* welp. no shallow clones in libgit2. */
        /* https://github.com/libgit2/libgit2/issues/3058 */
    }

    if (chdir(pkg->path[0]) != 0) {
        die("Repository directory is not accessible");
    }

    if (access(src, F_OK) != -1) {
        msg("Found local source %s", src);
        err = snprintf(dest, PATH_MAX, "%s/%s", pkg->path[0], src);
    }

end:
    if (err < 1) {
        die("Source '%s' does not exist", file);
    }

    if (err > PATH_MAX) {
        die("Source path exceeds PATH_MAX");
    }
}

void pkg_source(package *pkg) {
    char line[LINE_MAX];
    char *tok;
    FILE *file;
    int i = 0;
    int err;

    source_init(pkg);

    if (chdir(pkg->path[0]) != 0) {
        die("Repository is not accessible (%s)", pkg->path[0]);
    }

    file = fopen("sources", "r");

    if (!file) {
        die("Failed to open sources file");
    }

    pkg->src_l = cntlines(file);

    /* empty sources file */
    if (pkg->src_l == 0) {
        goto end;
    }

    pkg->src = xmalloc((pkg->src_l + 1) * sizeof(char *));
    pkg->des = xmalloc((pkg->src_l + 1) * sizeof(char *));

    while (fgets(line, LINE_MAX, file)) {
        if (line[0] == '#' || line[0] == '\n') {
            continue;
        }

        if (i > pkg->src_l) {
            die("Mismatch in source parser");
        }

        tok = strtok(line, " 	\r\n");

        if (!tok) {
            die("Invalid sources file");
        }

        pkg->src[i] = xmalloc(PATH_MAX);
        pkg->des[i] = xmalloc(PATH_MAX);
        source_resolve(pkg, tok, pkg->src[i]);

        tok = strtok(NULL, " 	\r\n");

        /* ensure optional field is not null */
        err = strlcpy(pkg->des[i], tok ? tok : "", PATH_MAX);

        if (err > PATH_MAX) {
            die("strlcpy was truncated");
        }

        i++;
    }

end:
    fclose(file);
}
