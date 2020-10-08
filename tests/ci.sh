#!/bin/sh

export CFLAGS="$CFLAGS -Werror"

CC=gcc   make -B
CC=clang make -B

# tcc segfaults due to atexit()?
# CC=tcc   make -B
