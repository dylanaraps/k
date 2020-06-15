.POSIX:

PREFIX=/usr/local
ALL_CFLAGS=$(CFLAGS) -std=c99 -static -Wall -Wextra -Wpedantic
ALL_LDFLAGS=$(LDFLAGS) -larchive -llzma -lbz2 -lcurl -lssl -lcrypto -lcrypto -lz

OBJ=\
	src/pkg.o \
	src/repo.o \
	src/source.o \
	src/util.o \
	src/kiss.o

HDR=\
	src/repo.h \
	src/source.h \
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
