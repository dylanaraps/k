k
________________________________________________________________________________

kiss pkg man written in C99.


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


todo
________________________________________________________________________________


Big things.

- [ ] Finishing 'kiss b'.
    - [ ] Manifest generation.
    - [ ] Binary stripping.
    - [ ] Dependency finder.
    - [ ] Dependency resolution.
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
