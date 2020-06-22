k
________________________________________________________________________________

kiss pkg man written in c


dependencies
________________________________________________________________________________

- C99 compiler
- POSIX libc
- libarchive (gzip, xz, bzip2, whatever)
- curl (whatever SSL library)


building
________________________________________________________________________________

$ make
$ make DESTDIR="..." PREFIX="/usr" install


    Options (via CFLAGS).
    ____________________________________________________________________________

    -static          - Statically build the package manager (3MB statically
                       linked with musl) (44KB dynamically linked). This option
                       is recommended as it removes reliance on the system's
                       overall health.

    -DFREE_ON_EXIT   - Free memory at exit(). Modern operating systems will
                       free all memory regardless. Enable this for more safety
                       at the cost of exit efficiency. Also useful for
                       debugging memory issues during development.


todo
________________________________________________________________________________


Big things.

- [ ] Finishing 'kiss b'.
    - [ ] Manifest generation.
    - [ ] Binary stripping.
    - [ ] Dependency finder.
    - [ ] Dependency support.
- [ ] 'kiss i'
- [ ] 'kiss r'
- [ ] 'kiss a'
- [ ] 'kiss u'


The nitpicky stuff.

- [ ] Add support for KISS_PATH.
- [ ] Add support for meta-packages.
- [ ] Add support for Git sources.
- [ ] Find a better sha256 implementation.
