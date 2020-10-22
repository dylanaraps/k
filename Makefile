.POSIX:

PREFIX = /usr/local

BUILD_FLAGS = \
	-std=c99 \
	-Wall \
	-Wextra \
	-Wpedantic \
	-D_POSIX_C_SOURCE=200809L \
	$(CFLAGS)

VALGRIND = \
	--leak-check=full \
	--show-leak-kinds=all \
	--track-origins=yes \
	--error-exitcode=1 \
	--trace-children=yes

OBJ = \
	src/kiss.o

HDR = \


kiss: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

.c.o:
	$(CC) $(BUILD_FLAGS) -c -o $@ $<

$(OBJ): $(HDR)

check:
	valgrind $(VALGRIND) ./kiss
	valgrind $(VALGRIND) ./kiss d
	valgrind $(VALGRIND) ./kiss s zlib
	valgrind $(VALGRIND) ./kiss v
	valgrind $(VALGRIND) ./kiss b

compdb:
	ninja -t compdb cc > compile_commands.json

clean:
	rm -f kiss $(OBJ)

.PHONY: install clean check
