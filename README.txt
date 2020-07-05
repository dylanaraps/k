k
________________________________________________________________________________

The eventual replacements to 'kiss install' and 'kiss remove' written in POSIX
C99.


dependencies
________________________________________________________________________________

- C99 compiler
- POSIX libc


building
________________________________________________________________________________

$ make
$ make DESTDIR="..." PREFIX="/usr" install


    options (via CFLAGS)
    ____________________________________________________________________________

    -static         -  Statically build the package manager (recommended). This
                       adds 2-3MB to the resulting binary but removes its
                       reliance on overall system health.


todo
________________________________________________________________________________

- [ ] install
- [ ] remove
