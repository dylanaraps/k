.POSIX:

PREFIX=/usr/local
ALL_CFLAGS=$(CFLAGS) -g -O0 -std=c99 -static -Wall -Wextra -Wpedantic -DLOG_USE_COLOR
ALL_LDFLAGS=$(LDFLAGS) -larchive -llzma -lbz2 -lcurl -lssl -lcrypto -lcrypto -lz

OBJ=\
	src/log.o \
	src/util.o \
	src/build.o \
	src/checksum.o \
	src/extract.o \
	src/tar.o \
	src/find.o \
	src/list.o \
	src/repo.o \
	src/sha.o \
	src/source.o \
	src/version.o \
	src/pkg.o \
	src/kiss.o

HDR=\
	src/log.h \
	src/util.h \
	src/build.h \
	src/checksum.h \
	src/extract.h \
	src/tar.h \
	src/find.h \
	src/list.h \
	src/repo.h \
	src/sha.h \
	src/source.h \
	src/version.h \
	src/pkg.h

.c.o:
	$(CC) $(ALL_CFLAGS) -c -o $@ $< 

kiss: $(OBJ)
	$(CC) -DCURL_STATICLIB $(ALL_CFLAGS) -o $@ $(OBJ) $(ALL_LDFLAGS) 

$(OBJ): $(HDR)

install: kiss
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp kiss $(DESTDIR)$(PREFIX)/bin/

clean:
	rm -f kiss $(OBJ)

.PHONY: install clean
