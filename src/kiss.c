#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <curl/curl.h>

#include "find.h"
#include "source.h"

int main (int argc, char *argv[]) {
    char *dummy = path_query("xz"); 

    curl_global_init(CURL_GLOBAL_ALL);

    if (dummy)
        printf("%s\n", dummy);

    source_download("https://zlsib.net/zlib-1.2.11.tar.gz");

    return 0;
}
