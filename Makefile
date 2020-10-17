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
	-Wsign-conversion \
	-fno-omit-frame-pointer \

VALGRIND = \
	--leak-check=full \
	--show-leak-kinds=all \
	--track-origins=yes \
	--error-exitcode=1 \
	--trace-children=yes \
	--show-reachable=no \

OBJ = \
	src/kiss.o \
	src/str.o \
	src/util.o

HDR = \
	src/str.h \
	src/vec.h \
	src/util.h

kiss: $(OBJ)
	$(CC) $(BUILD_FLAGS) $(CFLAGS) -O0 -g -o $@ $(OBJ) $(LDFLAGS)

.c.o:
	$(CC) $(BUILD_FLAGS) $(CFLAGS) -O0 -g -c -o $@ $<

$(OBJ): $(HDR)

check:
	valgrind $(VALGRIND) ./kiss l

clean:
	rm -f kiss $(OBJ)

.PHONY: install clean check
