.POSIX:

PREFIX = /usr/local

XCFLAGS = \
	-std=c99 \
	-D_POSIX_C_SOURCE=200809L \
	-Wall \
	-Wextra \
	-pedantic \
	$(CFLAGS)

OBJ = \
	src/action/list.o \
	src/action/search.o \
	src/download.o \
	src/list.o \
	src/repo.o \
	src/str.o \
	src/test.o \
	src/kiss.o

HDR = \
	include/action.h \
	include/download.h \
	include/error.h \
	include/list.h \
	include/repo.h \
	include/str.h \
	include/test.h

.c.o:
	$(CC) -Iinclude -lcurl $(XCFLAGS) $(CPPFLAGS) -c -o $@ $<

kiss: $(OBJ)
	$(CC) -o $@ $(OBJ) -lcurl $(LDFLAGS)

$(OBJ): $(HDR)

install: kiss
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f kiss $(DESTDIR)$(PREFIX)/bin

compdb:
	ninja -t compdb cc > compile_commands.json

check: kiss
	./test/run

clean:
	rm -f kiss $(OBJ)

.PHONY: install clean check
