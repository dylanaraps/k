#ifndef KISS_FILE_H
#define KISS_FILE_H

int copy_file(const char *src, const char *dest);
int copy_stream(FILE *r, FILE *w);

#endif
