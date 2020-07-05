.POSIX:

PREFIX       = /usr/local
ALL_WARNINGS = -Wall -Wextra -pedantic -Wmissing-prototypes -Wstrict-prototypes
ALL_CFLAGS   = $(CFLAGS) $(CPPFLAGS) -O0 -g -static -std=c99 $(ALL_WARNINGS)
ALL_LDFLAGS  = $(LDFLAGS) $(LIBS)

OBJ = src/kiss.o
HDR =

.c.o:
	$(CC) $(ALL_CFLAGS) -c -o $@ $<

kiss: $(OBJ)
	$(CC) $(ALL_CFLAGS) -o $@ $(OBJ) $(ALL_LDFLAGS)

$(OBJ): $(HDR)

install: kiss
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp kiss  $(DESTDIR)$(PREFIX)/bin/

clean:
	rm -f kiss $(OBJ)

.PHONY: install clean
