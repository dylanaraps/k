.POSIX:

PREFIX = /usr/local

ALL_CFLAGS = \
	-std=c99 \
	-D_POSIX_C_SOURCE=200809L \
	-Wall \
	-Wextra \
	-pedantic \
	-Wmissing-prototypes \
	-Wstrict-prototypes \
	-O3 \
	$(CFLAGS)

VALGRIND = \
	--leak-check=full \
	--show-leak-kinds=all \
	--suppressions=./tests/musl.supp \
	--track-origins=yes \
	--error-exitcode=1 \
	--trace-children=yes

OBJ = \
	src/kiss.o \
	src/util.o \
	src/repo.o \
	src/str.o \
	src/pkg.o

HDR = \
	src/util.h \
	src/str.h \
	src/repo.h \
	src/vec.h \
	src/pkg.h

kiss: $(OBJ)
	$(CC) $(ALL_CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

.c.o:
	$(CC) $(ALL_CFLAGS) -c -o $@ $<

$(OBJ): $(HDR)

check:
	valgrind $(VALGRIND) ./kiss
	valgrind $(VALGRIND) ./kiss s zlib
	valgrind $(VALGRIND) ./kiss l xz
	valgrind $(VALGRIND) ./kiss v
	valgrind $(VALGRIND) ./kiss b

clean:
	rm -f kiss $(OBJ)

.PHONY: install clean check
