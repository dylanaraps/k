.POSIX:

PREFIX = /usr/local

kiss:
	./make

install: kiss
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f kiss $(DESTDIR)$(PREFIX)/bin

check: kiss
	./make check

.PHONY: kiss install check
