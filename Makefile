.POSIX:

PREFIX       = /usr/local
ALL_WARNINGS = -Wall -Wextra -pedantic -Wmissing-prototypes -Wstrict-prototypes
ALL_CFLAGS   = -std=c99 $(ALL_WARNINGS) -O3 $(CFLAGS) $(CPPFLAGS)

OBJ = \
	src/kiss.o \
	src/util.o \
	src/str.o

HDR = \
	src/util.h \
	src/str.h \
	src/vec.h

.c.o:
	$(CC) $(ALL_CFLAGS) -c -o $@ $<

kiss: $(OBJ)
	$(CC) $(ALL_CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

$(OBJ): $(HDR)

clean:
	rm -f kiss $(OBJ)

.PHONY: install clean
