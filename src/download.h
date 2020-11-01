#ifndef KISS_DOWNLOAD_H
#define KISS_DOWNLOAD_H

int source_download(const char *url, FILE *dest);
void source_curl_cleanup(void);

#endif
