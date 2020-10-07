.POSIX:

PREFIX = /usr/local

ALL_CFLAGS = \
	-std=c99 \
	-Wall \
	-Wextra \
	-pedantic \
	-Wmissing-prototypes \
	-Wstrict-prototypes \
	-O3 \
	$(CFLAGS)

VALGRIND = -s \
	--leak-check=full \
	--show-leak-kinds=all \
	--suppressions=./tests/musl.supp \
	--track-origins=yes \
	--error-exitcode=1

OBJ = \
	src/kiss.o \
	src/util.o \
	src/str.o

kiss:
	$(CC) $(ALL_CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

check:
	valgrind -s $(VALGRIND) ./kiss s zlib
	valgrind -s $(VALGRIND) ./kiss l xz
	valgrind -s $(VALGRIND) ./kiss v

clean:
	rm -f kiss $(OBJ)

.PHONY: install clean check
