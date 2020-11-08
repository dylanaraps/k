.POSIX:

PREFIX = /usr/local

kiss:
	./build

install: kiss
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f kiss $(DESTDIR)$(PREFIX)/bin

check: kiss
	./test/run

clean:
	rm -f kiss

.PHONY: kiss install check clean
