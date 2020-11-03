.POSIX:

PREFIX = /usr/local

BUILD_CFLAGS = \
	-std=c99 \
	-D_POSIX_C_SOURCE=200809L \
	-Wall \
	-Wextra \
	-pedantic \
	-Iinclude \
	$(CFLAGS) \
	$(CPPFLAGS)

BUILD_LDFLAGS = \
	-lcurl \
	$(LDFLAGS)

VALGRIND = \
	--leak-check=full \
	--show-leak-kinds=all \
	--track-origins=yes \
	--error-exitcode=1

OBJ = \
	src/download.o \
	src/list.o \
	src/str.o \
	src/kiss.o

HDR = \
	include/download.h \
	include/error.h \
	include/list.h \
	include/str.h

.c.o:
	$(CC) $(BUILD_CFLAGS) -c -o $@ $<

kiss: $(OBJ)
	$(CC) -o $@ $(OBJ) $(BUILD_LDFLAGS)

$(OBJ): $(HDR)

install: kiss
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f kiss $(DESTDIR)$(PREFIX)/bin

check:
	valgrind $(VALGRIND) ./kiss
	valgrind $(VALGRIND) ./kiss s zlib

compdb:
	ninja -t compdb cc > compile_commands.json

clean:
	rm -f kiss $(OBJ)

.PHONY: install clean check
