.POSIX:

PREFIX=/usr/local
ALL_CFLAGS=$(CFLAGS) -static -std=c99 -Wall -Wextra -Wpedantic
ALL_LDFLAGS=$(LDFLAGS) -larchive -llzma -lbz2 -lcurl -lssl -lcrypto -lz

OBJ=\
	src/build.o \
	src/signal.o \
	src/file.o \
	src/extract.o \
	src/source.o \
	src/checksum.o \
	src/sha256.o \
	src/repo.o \
	src/cache.o \
	src/find.o \
	src/list.o \
	src/version.o \
	src/log.o \
	src/util.o \
	src/pkg.o \
	src/kiss.o

HDR=\
	src/build.h \
	src/signal.h \
	src/file.h \
	src/extract.h \
	src/source.h \
	src/checksum.h \
	src/sha256.h \
	src/repo.h \
	src/cache.h \
	src/find.h \
	src/list.h \
	src/version.h \
	src/log.h \
	src/util.h \
	src/pkg.h

.c.o:
	$(CC) $(ALL_CFLAGS) -c -o $@ $<

kiss: $(OBJ)
	$(CC) $(ALL_CFLAGS) -o $@ $(OBJ) $(ALL_LDFLAGS)

$(OBJ): $(HDR)

install: kiss
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp kiss $(DESTDIR)$(PREFIX)/bin/

clean:
	rm -f kiss $(OBJ)

.PHONY: install clean
