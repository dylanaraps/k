.POSIX:

PREFIX = /usr/local

BUILD_CFLAGS = \
	-std=c99 \
	-D_POSIX_C_SOURCE=200809L \
	-Wall \
	-Wextra \
	-pedantic \
	$(CFLAGS) \
	$(CPPFLAGS)

BUILD_LDFLAGS = \
	-lcurl \
	$(LDFLAGS)

VALGRIND = \
	--leak-check=full \
	--show-leak-kinds=all \
	--track-origins=yes \
	--error-exitcode=1 \
	--track-fds=yes \

OBJ = \
	src/download.o \
	src/repo.o \
	src/str.o \
	src/kiss.o

HDR = \
	src/download.h \
	src/error.h \
	src/repo.h \
	src/str.h

.c.o:
	$(CC) $(BUILD_CFLAGS) -g -O0 -c -o $@ $<

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
