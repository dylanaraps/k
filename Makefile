.POSIX:

PREFIX=/usr/local
ALL_CFLAGS=$(CFLAGS) -std=c99 -Wall -Wextra -Wpedantic

OBJ=\
	src/find.o \
	src/source.o \
	src/kiss.o

HDR=\
	src/find.h \
	src/source.h \
	src/kiss.c

.c.o:
	$(CC) $(ALL_CFLAGS) -c -o $@ $< -lcurl

kiss: $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) -lcurl

$(OBJ): $(HDR)

install: kiss
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp kiss $(DESTDIR)$(PREFIX)/bin/

clean:
	rm -f kiss $(OBJ)

.PHONY: install clean
