#!/bin/sh -e

configure() {
    CFLAGS="-Wall -Wextra -pedantic $CFLAGS"
    CFLAGS="-std=c99 -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700 $CFLAGS"
    CPPFLAGS="-Iinclude $CPPFLAGS"

    _dep() {
        pkg-config --libs --cflags "${static:+--static}" "$1" ||
        printf '%s\n' "$2"
    }

    # Detect when '-static' is passed via LDFLAGS and handle things accordingly.
    case " $LDFLAGS " in *" -static "*)
        static=1
    esac

    # Download functionality. Setting the environment variable CURL to '0' will
    # disable the package manager's ability to download sources. Git sources
    # will continue to work as normal (so long as git does).
    #
    # NOTE: The package manager does not depend on a specific SSL library. It
    #       only directly depends on libcurl.
    case ${CURL:=1} in 1)
        LDFLAGS="$(_dep libcurl -lcurl) $LDFLAGS"
        CFLAGS="-DUSE_CURL $CFLAGS"
    esac

    # Which sha256 implementation to use. If all options are '0', an internal
    # sha256 implementation will be used. NOTE: Only one option may be enabled
    # at a time.
    {
        # SHA256 external implementation. Setting the environment variable
        # BEARSSL to '1' will cause the package manager to use sha256 from
        # bearssl.
        case ${BEARSSL:=0} in 1)
            LDFLAGS="$(_dep bearssl '-lbearssl') $LDFLAGS"
            CFLAGS="-DUSE_BEARSSL $CFLAGS"
        esac

        # SHA256 external implementation. Setting the environment variable
        # OPENSSL to '1' will cause the package manager to use sha256 from
        # openssl.
        case ${OPENSSL:=0} in 1)
            LDFLAGS="$(_dep openssl '-lssl -lcrypto') $LDFLAGS"
            CFLAGS="-DUSE_OPENSSL $CFLAGS"
        esac
    }

    # tar external implementation. Setting the environment variable
    # LIBARCHIVE to '1' will cause the package manager to use tar from
    # libarchive. If set to '0', the tar command will be executed.
    case ${LIBARCHIVE:=0} in 1)
        LDFLAGS="$(_dep libarchive '-larchive') $LDFLAGS"
        CFLAGS="-DUSE_LIBARCHIVE $CFLAGS"
    esac
}

build() {
    configure

    _cc() {
        printf '%s %s\n' "${CC:=cc}" "$*"
        "$CC" "$@" || exit 1
    }

    for obj in src/[!k]*.c src/*/*.c; do
        _cc $CFLAGS $CPPFLAGS -c -o "${obj%%.c}.o" "$obj"
    done

    for prog in src/kiss.c test/*.c; do
        _cc $CFLAGS $CPPFLAGS -o "${prog%%.c}" "$prog" \
            src/[!k]*.o src/*/*.o $LDFLAGS
    done
}

check() {
    export KISS_ROOT=$PWD/test/test_hier
    export KISS_PATH=$KISS_ROOT/repo/core:$KISS_ROOT/repo/extra
    export XDG_CACHE_HOME=$PWD/test/test_hier

    command -v valgrind &&
        set -- valgrind \
        --leak-check=full \
        --track-origins=yes \
        --error-exitcode=1 \
        --suppressions=test/valgrind/musl.supp \
        --suppressions=test/valgrind/lzma.supp

    for file in test/*.c; do
        "$@" "${file%%.c}" || exit 1
    done
}

compdb() {
    configure

    printf '[\n'
    for obj in src/*.c src/*/*.c; do
cat <<EOF
    {
        "directory": "$PWD",
        "command": "${CC:=cc} $CFLAGS $CPPFLAGS -c -o ${obj%%.c}.o $obj",
        "file": "$obj"
    },
EOF
    done
    printf ']\n'
} > compile_commands.json

main() {
    "${1:-build}" || {
        printf 'error during %s\n' "${1:-build}" >&2
        exit 1
    }
}

main "$@"
