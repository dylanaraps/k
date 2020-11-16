/**
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2020 Dylan Araps
 */
#ifndef KISS_DOWNLOAD_H
#define KISS_DOWNLOAD_H

#include <stdio.h>

/**
 * Download a URL to the given file pointer. This function will initialize curl
 * on first use. Subsequent calls reuse the same curl handle. Returns 0 for
 * success and -1 for error.
 *
 * On error, any written data is not removed. This must be done by the caller.
 */
int source_download(const char *url, FILE *dest);

/**
 * Call the global curl clean up functions. This should only be called a single
 * time and only once curl is no longer needed.
 */
void source_curl_cleanup(void);

#endif
