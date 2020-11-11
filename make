#!/bin/sh

dep() {
    pkg-config --libs --cflags "${static:+--static}" "$1" ||
    printf '%s\n' "$2"
}

_cc() {
    printf '%s %s\n' "${CC:=cc}" "$*"
    "$CC" "$@"
}

configure() {
    CFLAGS="-Wall -Wextra -pedantic $CFLAGS"
    CFLAGS="-std=c99 -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700 $CFLAGS"
    CPPFLAGS="-Iinclude $CPPFLAGS"

    # Detect when '-static' is passed via LDFLAGS and handle things accordingly.
    case " $LDFLAGS " in *" -static "*)
        static=1
    esac

    # Download functionality. Setting the environment variable CURL to '0' will
    # disable the package manager's ability to download sources. Git sources
    # will continue to work as normal (so long as git does).
    case ${CURL:=1} in 1)
        LDFLAGS="$(dep libcurl -lcurl) $LDFLAGS"
        CFLAGS="-DUSE_CURL $CFLAGS"
    esac

    # SHA256 external implementation. Setting the environment variable OPENSSL
    # to '0' will cause the package manager to use an internal sha256
    # implementation rather than the optimized ASM from openssl.
    case ${OPENSSL:=1} in 1)
        LDFLAGS="$(dep openssl '-lssl -lcrypto') $LDFLAGS"
        CFLAGS="-DUSE_OPENSSL $CFLAGS"
    esac
}

build() {
    configure

    for obj in src/*.c src/*/*.c; do
        _cc $CFLAGS $CPPFLAGS -c -o "${obj%%.c}.o" "$obj"
    done

    for obj in test/*.c; do
        _cc $CFLAGS $CPPFLAGS -o "${obj%%.c}" "$obj" \
            src/[!k]*.o src/*/*.o $LDFLAGS
    done

    _cc $CFLAGS $CPPFLAGS -o kiss src/*.c src/*/*.c $LDFLAGS
}

check() {
    export KISS_PATH=$PWD/test/test_repo/core:$PWD/test/test_repo/extra
    export XDG_CACHE_HOME=$PWD/test/test_hier

    command -v valgrind &&
        set -- valgrind --leak-check=full --track-origins=yes --error-exitcode=1

    for file in test/*.c; do
        "$@" "${file%%.c}"
    done

    "$@" ./kiss v
    "$@" ./kiss
    "$@" ./kiss s zlib
}

set -e

"${1:-build}" "$2"
