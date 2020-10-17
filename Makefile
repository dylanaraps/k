.POSIX:

PREFIX = /usr/local

BUILD_FLAGS = \
	-std=c99 \
	-D_POSIX_C_SOURCE=200809L \
	-Wall \
	-Wextra \
	-pedantic \
	-Wmissing-prototypes \
	-Wstrict-prototypes \
	-Wwrite-strings \
	-Wshadow \
	-Wundef \
	-Wsign-conversion

VALGRIND = \
	--leak-check=full \
	--show-leak-kinds=all \
	--track-origins=yes \
	--error-exitcode=1 \
	--trace-children=yes

OBJ = \
	src/kiss.o \
	src/cache.o \
	src/str.o \
	src/util.o

HDR = \
	src/cache.h \
	src/str.h \
	src/vec.h \
	src/util.h

kiss: $(OBJ)
	$(CC) $(BUILD_FLAGS) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

.c.o:
	$(CC) $(BUILD_FLAGS) $(CFLAGS) -c -o $@ $<

$(OBJ): $(HDR)

check:
	valgrind $(VALGRIND) ./kiss
	valgrind $(VALGRIND) ./kiss d
	valgrind $(VALGRIND) ./kiss s zlib
	valgrind $(VALGRIND) ./kiss v
	valgrind $(VALGRIND) ./kiss b

clean:
	rm -f kiss $(OBJ)

.PHONY: install clean check
