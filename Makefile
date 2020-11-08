.POSIX:

PREFIX = /usr/local

kiss:
	./build

install: kiss
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f kiss $(DESTDIR)$(PREFIX)/bin

check: kiss
	./test/run

.PHONY: kiss install check
