k
________________________________________________________________________________

kiss pkg man written in C99. [wip]


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


    options (via CFLAGS)
    ____________________________________________________________________________

    -static         -  Statically build the package manager (recommended). This
                       adds 2-3MB to the resulting binary but removes its
                       reliance on overall system health.


todo
________________________________________________________________________________

Big things.

- [ ] Finishing 'kiss b'.
    - [ ] Manifest generation.
    - [ ] Binary stripping.
    - [ ] Dependency finder.
    - [ ] Dependency resolution.
    - [ ] Tarball creation.
- [ ] 'kiss i'
- [ ] 'kiss r'
- [ ] 'kiss a'
- [ ] 'kiss u'
- [x] 'kiss s'
- [x] 'kiss l'
- [x] 'kiss d'
- [x] 'kiss c'
- [x] 'kiss v'
- [ ] Elevation of privileges.

The nitpicky stuff.

- [ ] Add support for KISS_PATH.
- [ ] Add support for meta-packages.
- [ ] Add support for Git sources.
- [ ] Find a better sha256 implementation.
- [ ] Add support for globbing in 'kiss s'.
- [ ] Log build output to file (via dup() or dup2()).
- [ ] Permission and ownership support in (src/file.c).
- [ ] Figure out how messages should be displayed.
- [ ] Drop nftw() as it is XSI and not POSIX.
- [ ] Swap to vector instead of linked list.
- [ ] Move away from libarchive.
