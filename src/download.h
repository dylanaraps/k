#ifndef KISS_DOWNLOAD_H
#define KISS_DOWNLOAD_H

int source_download(const char *url, int dest_fd);
int source_type(const char *url, int dest_fd, int repo_fd);

enum src_type {
    SRC_URL,
    SRC_GIT,
    SRC_CAC,
    SRC_ABS,
    SRC_REL,
    SRC_ENOENT,
};

#endif
