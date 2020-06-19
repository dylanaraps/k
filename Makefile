.POSIX:

PREFIX=/usr/local
ALL_CFLAGS=$(CFLAGS) -g -O0 -std=c99 -static -Wall -Wextra -Wpedantic -W -Werror
ALL_LDFLAGS=$(LDFLAGS) -larchive -llzma -lbz2 -lcurl -lssl -lcrypto -lcrypto -lz

OBJ=\
	src/repo.o \
	src/find.o \
	src/list.o \
	src/version.o \
	src/log.o \
	src/util.o \
	src/pkg.o \
	src/kiss.o

HDR=\
	src/repo.h \
	src/find.h \
	src/list.h \
	src/version.h \
	src/log.h \
	src/util.h \
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
