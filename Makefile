.POSIX:

PREFIX = /usr/local

XCFLAGS = \
	-std=c99 \
	-D_POSIX_C_SOURCE=200809L \
	-Wall \
	-Wextra \
	-pedantic \
	-Iinclude \
	$(CFLAGS) \
	$(CPPFLAGS)

XLDFLAGS = \
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
	src/action/list.o \
	src/kiss.o

HDR = \
	include/action.h \
	include/download.h \
	include/error.h \
	include/list.h \
	include/str.h

.c.o:
	$(CC) $(XCFLAGS) -c -o $@ $<

kiss: $(OBJ)
	$(CC) -o $@ $(OBJ) $(XLDFLAGS)

$(OBJ): $(HDR)

install: kiss
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f kiss $(DESTDIR)$(PREFIX)/bin

check:
	valgrind $(VALGRIND) ./kiss

compdb:
	ninja -t compdb cc > compile_commands.json

clean:
	rm -f kiss $(OBJ)

.PHONY: install clean check
